# Phase 8 â€“ Change Request CR1: Whole-Class Deadline Extension

## 1. Change request
A lecturer needs to extend an assignment deadline for the whole class. Every enrolled student must receive the revised effective deadline, and the revised deadline must be visible in the student's assignment list.

## 2. Maintenance classification
**Perfective maintenance.** CR1 adds/improves user-visible functionality rather than correcting a previously documented failure or adapting the system to a new platform. The change improves the lecturer's ability to manage exceptional class-wide circumstances and helps students see the correct deadline.

## 3. Requirements affected
- Lecturer can grant a whole-class extension only for an assignment they own.
- A valid extension must be later than the original deadline and include a reason.
- The extension applies to students enrolled in the assignment's course.
- Deadline and late-penalty calculations use the latest applicable extension for each student (whole-class or individual).
- The student assignment-list service returns both original and effective deadlines and identifies when the deadline has changed.
- An individual extension remains specific to the named student.

## 4. Design and implementation impact

| Element | Change |
|---|---|
| `whole_class_extensions` table | Stores assignment, revised deadline, lecturer, approval timestamp and reason. |
| `WorkflowService::grantClassExtension` | Validates assignment ownership, lecturer role, later deadline and reason; stores the class extension. |
| `WorkflowService::effectiveDeadline` | Resolves the latest applicable individual/class extension for a student. |
| `StudentAssignment` | DTO containing assignment details, original deadline, effective deadline, maximum mark and extension indicator. |
| `WorkflowService::listAssignmentsForStudent` | Returns assignments for the student's enrolled courses with the correct effective deadline. |
| `WorkflowService::submit` / `recordMark` | Continue using the effective deadline so submission status and penalties agree with the student list. |

### Data flow
1. Lecturer submits the assignment ID, revised deadline and reason.
2. Service verifies that the caller is the active lecturer who owns the assignment.
3. Service validates the revised deadline and reason, then stores the class extension.
4. Student requests their assignment list.
5. Service selects assignments from the student's enrolled courses and resolves the effective deadline for each row.
6. The caller can display the returned effective deadline; `deadlineExtended` indicates that it differs from the original.

## 5. Regression test cases

| Test | Expected result |
|---|---|
| Class extension applied to assignment | All enrolled students resolve to the revised deadline. |
| Student assignment list after class extension | List returns revised effective deadline and `deadlineExtended=true`. |
| Individual extension | Only the selected student sees the individual revised deadline. |
| Other student without individual extension | Original deadline remains unless a class extension also applies. |
| Non-student requests student list | Request rejected and result list remains empty. |
| Submission after extension but before revised deadline | Not treated as late. |
| Existing late-penalty boundary tests | Existing 1-day, 3-day and over-3-day rules remain unchanged relative to effective deadline. |

Automated regression tests for the first five cases have been added to `tests/WorkflowServiceTests.cpp`; existing submission/penalty tests remain in the same suite. Execution evidence must be recorded after running the full suite on the configured Windows/MSYS2 environment.

## 6. Release and documentation impact
This change is intended for the v1.1 maintenance release. Update the student manual to explain the effective deadline field, lecturer manual to explain class-wide extensions, Phase 5 traceability/test records, and release notes. The existing v1.0 tag must remain unchanged; create a v1.1 Git tag only after local build, all automated tests, manual regression checks and review have passed.

## 7. Known limitation
The project is still a backend/service prototype. The new student assignment-list query is implemented, but the interactive student menu/screen that renders the returned rows is not yet present. Therefore the data needed for the student-visible deadline is available to a UI, but a complete visual student list is not claimed as delivered.
