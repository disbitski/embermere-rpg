"""Host regression for stable value-based preservation before map mutation."""

from pathlib import Path
import runpy
from types import ModuleType, SimpleNamespace
import unittest
from unittest.mock import patch


class ActorFixture:
    def __init__(self):
        self.location = SimpleNamespace(x=1,y=2,z=3)
        self.rotation = SimpleNamespace(pitch=0,yaw=15,roll=0)
        self.scale = SimpleNamespace(x=1,y=1,z=1)
        self.tags = ['Owned']
        self.calls = 0
        self.component = SimpleNamespace(
            static_mesh=SimpleNamespace(get_path_name=lambda:'/Game/TestMesh'),
            get_collision_profile_name=lambda:'BlockAll',
            get_collision_enabled=lambda:'QueryAndPhysics',
            get_materials=lambda:[SimpleNamespace(get_path_name=lambda:'/Game/TestMaterial')])

    def get_actor_transform(self):
        self.calls += 1
        return 'Temporary struct address '+str(self.calls)

    def get_actor_location(self):
        return self.location

    def get_actor_rotation(self):
        return self.rotation

    def get_actor_scale3d(self):
        return self.scale

    def get_folder_path(self):
        return 'Owned/Folder'

    def get_component_by_class(self,cls):
        return self.component


class PreservationTests(unittest.TestCase):
    def setUp(self):
        stub = ModuleType('unreal')
        stub.StaticMeshComponent = object
        with patch.dict('sys.modules',{'unreal':stub}):
            self.signature = runpy.run_path(str(Path(__file__).with_name('place_marshward_foundation_unreal.py')))['signature']

    def test_temporary_transform_addresses_do_not_change_signature(self):
        actor = ActorFixture()
        self.assertEqual(self.signature(actor),self.signature(actor))
        self.assertEqual(actor.calls,0)

    def test_actual_actor_drift_is_detected(self):
        mutations = [lambda a:setattr(a.location,'z',4),lambda a:setattr(a.rotation,'yaw',90),
                     lambda a:setattr(a.scale,'x',2),lambda a:a.tags.append('Other'),
                     lambda a:setattr(a.component,'get_collision_profile_name',lambda:'NoCollision'),
                     lambda a:setattr(a.component,'get_materials',lambda:[])]
        for mutate in mutations:
            with self.subTest(mutation=mutate):
                actor = ActorFixture()
                before = self.signature(actor)
                mutate(actor)
                self.assertNotEqual(before,self.signature(actor))


if __name__ == '__main__':
    unittest.main()
