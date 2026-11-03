# Phase 5 â€“ Verification and Validation Execution Status

## Scope

This report records the automated test suites currently included in the project and distinguishes user-reported results from independently reproduced results. It does not claim that manual system or acceptance tests have been executed.

## Automated suites

| Suite | Test source | Test cases in source | Latest result available |
|---|---|---:|---|
| Workflow | `tests/WorkflowServiceTests.cpp` | 17 | User reported all CTest tests passed on the Windows development machine (1 Nov 2026) |
| Authentication/repository integration | `tests/AuthenticationServiceTests.cpp` | 7 | User reported all CTest tests passed on the Windows development machine (1 Nov 2026) |
| Password hashing | `tests/PasswordHasherTests.cpp` | 9 | User reported all CTest tests passed on the Windows development machine (1 Nov 2026) |

The combined source count is 33 GoogleTest cases. The archived build directory predates registration of all three targets; the corrected `CMakeLists.txt` in this project now registers all three suites. Reconfigure and rebuild before treating the archive itself as verified.

## Coverage evidence

The user supplied the following GCC gcov output from the Windows development environment after running the three suites:

| Source file | Executed lines | Total lines | Line coverage |
|---|---:|---:|---:|
| `src/services/PasswordHasher.cpp` | 42 | 52 | 80.77% |
| `src/services/AuthenticationService.cpp` | 26 | 26 | 100.00% |
| `src/services/WorkflowService.cpp` | 220 | 227 | 96.92% |
| **Combined measured service files** | **288** | **305** | **94.43%** |

Calculation: `(42 + 26 + 220) / (52 + 26 + 227) Ã— 100 = 94.43%`.

This is the combined line coverage of the three measured service implementation files only. It is not a claim about every repository or every file that the lecturer may define as part of the business-logic layer. The aggregate gcov percentages at the bottom of individual commands included C++ standard-library headers and must not be reported as project coverage.

## Manual system and acceptance testing

Manual system and acceptance test cases are documented in `system_tests/PHASE5_SYSTEM_ACCEPTANCE_TESTS.md` and `MANUAL_TEST_PLAN.md`. Their execution results remain **Pending** until a tester records the actual observed result, evidence and date. In particular, the archive does not contain a complete interactive role-based application through which all user acceptance scenarios can currently be demonstrated.

## Exit assessment

- Automated suite source files: present.
- CMake registration: corrected in this revision; reconfigure/build required.
- User-reported automated test execution: passed.
- Measured coverage of the three service files: 94.43%, above the 70% target for that measured scope.
- Full business-logic-layer coverage: requires confirmation of the lecturer's layer definition and any additional included files.
- Manual system/acceptance execution: pending.
- Defect retest evidence: pending; do not invent successful retest outcomes.

**Phase 5 is not declared fully complete** until manual tests, defect retests, and exact requirement traceability are completed and evidenced.
