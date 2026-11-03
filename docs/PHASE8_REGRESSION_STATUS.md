# Phase 8 â€“ Maintenance and Regression Status

## CR1 implementation
- [x] Whole-class extension persistence in `whole_class_extensions`.
- [x] Lecturer ownership/role, later-deadline and reason validation.
- [x] Effective-deadline resolution for class-wide and individual extensions.
- [x] Student assignment-list service query restricted to the student's enrolled courses.
- [x] Assignment-list result includes original deadline, effective deadline and extension indicator.
- [x] Regression tests added for class-wide display, individual-extension isolation and role rejection.
- [x] Change-request, impact-analysis and release documentation prepared.

## Verification status
The source and test changes have been prepared in this package. They have **not** been compiled or executed in this environment because the Linux workspace does not contain the required libsodium development header. Run the build and full CTest suite on the configured Windows/MSYS2 machine and save the resulting logs before claiming a passing v1.1 regression run.

## Remaining limitations
- The interactive student assignment-list screen/menu is not implemented. `listAssignmentsForStudent` supplies the correct data for that screen, but the screen itself remains outstanding.
- Manual end-to-end regression on lecturer and student workflows remains outstanding.
- Do not create/tag v1.1 until build, tests, manual checks and review have passed.
