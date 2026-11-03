# Phase 5 Defect Log

| ID | Severity | Defect / observed behaviour | Cause | Fix | Retest result | Status |
|---|---|---|---|---|---|---|
| D-001 | Medium (test-build configuration) | Linking `PasswordHasherTests.exe` failed with undefined references to `__gcov_init`, `__gcov_exit` and `__gcov_merge_add`. | `lms_core` was compiled with GCC coverage instrumentation, but the new test executable was not linked with the coverage runtime. | Added `target_link_options(PasswordHasherTests PRIVATE --coverage)` inside the GNU coverage configuration block. | Student subsequently reported successful CMake build and that all three CTest suites passed. | Fixed; retest passed (student-reported) |

## Defect-log guidance

Record each additional reproducible defect with an ID, severity, observed behaviour, cause, correction, retest result and status. Do not mark a defect fixed until the corresponding retest has actually passed. The entry above records the build issue observed during Phase 5 test integration; it does not imply that all defects have been discovered.
