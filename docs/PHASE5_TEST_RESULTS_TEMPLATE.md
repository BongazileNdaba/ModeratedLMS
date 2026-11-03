# Phase 5 ÃƒÂ¢Ã¢â€šÂ¬Ã¢â‚¬Å“ Test Results and Coverage Report

**Project:** Moderated LMS
**Test environment:** Windows 10, PowerShell, MSYS2 UCRT64 / MinGW GCC 16.2.0, CMake, GoogleTest 1.15.2
**Execution date:** 1 November 2026
**Evidence source:** Results and gcov output reported by the student from the target Windows development environment.

## 1. Automated test execution

The student reported that `ctest --test-dir build --output-on-failure` completed successfully after all three test executables were registered and built.

| Suite                                     | Test source                            | Test cases in source | Reported result             |
| ----------------------------------------- | -------------------------------------- | -------------------: | --------------------------- |
| Workflow                                  | `tests/WorkflowServiceTests.cpp`       |                   17 | Passed                      |
| Authentication and repository integration | `tests/AuthenticationServiceTests.cpp` |                    7 | Passed                      |
| Password hashing                          | `tests/PasswordHasherTests.cpp`        |                    9 | Passed                      |
| **Total**                                 |                                        |               **33** | **All three suites passed** |

These are user-reported execution results; this report does not represent an independent execution by the report author.

## 2. Line coverage results

Coverage was measured using GCC `gcov` against the three service implementation files after the test suites were run.

| Source file                              | Lines executed | Executable lines | Line coverage |
| ---------------------------------------- | -------------: | ---------------: | ------------: |
| `src/services/PasswordHasher.cpp`        |             42 |               52 |        80.77% |
| `src/services/AuthenticationService.cpp` |             26 |               26 |       100.00% |
| `src/services/WorkflowService.cpp`       |            220 |              227 |        96.92% |
| **Combined measured service files**      |        **288** |          **305** |    **94.43%** |

Calculation: `(42 + 26 + 220) / (52 + 26 + 227) ÃƒÆ’Ã¢â‚¬â€ 100 = 94.43%`.

**Coverage target:** at least 70% line coverage. The measured combined coverage of these three service files exceeds that target.

**Scope qualification:** 94.43% is the aggregate for the three measured service `.cpp` files only. `Database.cpp` and `UserRepository.cpp` were not included in this aggregate. Confirm whether the lecturer's definition of the business-logic layer includes those repository files before presenting 94.43% as coverage of the entire business-logic layer. The low aggregate percentages printed by individual `gcov` commands include C++ standard-library headers and are not the project-service coverage figure.

## 3. Manual system and acceptance test execution

Manual cases are specified in `system_tests/PHASE5_SYSTEM_ACCEPTANCE_TESTS.md` and `MANUAL_TEST_PLAN.md`. They have **not yet been evidenced as executed** in the supplied project record. Record tester, date, actual result, pass/fail and evidence for each case after execution. Do not mark these cases passed based solely on automated service tests.

| Test group       | Current status | Required next action                                                                 |
| ---------------- | -------------- | ------------------------------------------------------------------------------------ |
| System tests     | Pending        | Execute complete role-based workflows in the running application and record evidence |
| Acceptance tests | Pending        | Execute use-case acceptance scenarios and record evidence                            |

## 4. Defect log

See `DEFECT_LOG.md`. The observed CMake coverage-runtime linking issue for `PasswordHasherTests` was corrected by adding `target_link_options(PasswordHasherTests PRIVATE --coverage)`. The student subsequently reported a successful build and passing CTest run. No claim is made that all possible defects have been found.

## 5. Exit assessment

- Test plan and strategy: documented.
- Automated test code: present (33 test cases across three suites).
- Automated execution: student-reported pass for all three suites.
- Measured service-file coverage: 94.43% across the three listed services.
- Traceability matrix: present; verify against the original approved Phase 2/3 artefacts before final submission.
- Manual system and acceptance test results: pending execution and evidence.
- Defect log: updated for the observed build/link issue; further defects to be recorded if found.

**Phase 5 is not yet fully closed** until manual system/acceptance tests are executed, remaining relevant business-logic files are assessed for coverage, and the traceability matrix is reconciled with the approved requirements.
