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

    def test_boolean_slots_are_rejected_but_integer_zero_and_one_remain_valid(self):
        self.assertEqual(material_owners([('Basin', 0, '/peat'), ('Bank', 1, '/moss')]),
                         {'/moss': [['Bank', 1]], '/peat': [['Basin', 0]]})
        for slot in (False, True):
            with self.subTest(slot=slot), self.assertRaises(ValueError):
                material_owners([('Basin', slot, '/peat')])

    def test_non_integer_slots_are_rejected_without_coercion(self):
        for slot in (0.0, 1.0, '0', '1', None):
            with self.subTest(slot=slot), self.assertRaises(ValueError):
                material_owners([('Basin', slot, '/peat')])


if __name__ == '__main__':
    unittest.main()
