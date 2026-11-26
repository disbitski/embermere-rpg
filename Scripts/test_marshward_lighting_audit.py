"""Host checks for deterministic effective-material ownership reports."""

import unittest

from audit_marshward_lighting_unreal import material_owners


class LightingAuditTests(unittest.TestCase):
    def test_repeated_material_keeps_every_distinct_owner_slot(self):
        self.assertEqual(material_owners([('B', 1, '/moss'), ('A', 0, '/stone'),
                                          ('B', 0, '/moss')]),
                         {'/moss': [['B', 0], ['B', 1]], '/stone': [['A', 0]]})

    def test_duplicate_actor_slot_is_rejected_even_with_another_material(self):
        with self.assertRaisesRegex(ValueError, 'Duplicate owner slot'):
            material_owners([('Basin', 0, '/peat'), ('Basin', 0, '/pool')])

    def test_missing_material_or_invalid_slot_is_not_reported_as_success(self):
        for row in (('Basin', 0, ''), ('', 0, '/peat'), ('Basin', -1, '/peat')):
            with self.subTest(row=row), self.assertRaises(ValueError):
                material_owners([row])


if __name__ == '__main__':
    unittest.main()
