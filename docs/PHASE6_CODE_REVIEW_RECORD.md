# Phase 6 â€” Self Code Review Record

**Review scope:** CMake configuration, CI workflow, database/repository code, authentication/password hashing, workflow service, tests and project documentation.
**Review method:** checklist-based manual review of the uploaded source snapshot. This is a self-review record, not an independent peer review.

## Checklist and results

| Review question | Result | Evidence / action |
|---|---|---|
| Are responsibilities separated between service and repository layers? | Partial | Services and repositories exist, but `WorkflowService` issues SQL directly; consider extracting a repository for submissions/marks/extensions. |
| Are SQL statements parameterised? | Mostly yes | UserRepository and WorkflowService use prepared statements for values. Schema/migration SQL is static. |
| Are passwords stored as plaintext? | No | PasswordHasher uses libsodium Argon2id with random salt; AuthenticationService delegates verification. |
| Are authentication failures exposed with a generic message? | Yes | AuthenticationService returns a generic invalid-credentials error. |
| Are role and active-account checks performed in service logic? | Partial | Authentication and WorkflowService perform checks; full role-specific application flows are not implemented. |
| Are database errors distinguishable from ordinary absence/denial? | Needs improvement | Some WorkflowService helpers collapse SQL errors into `false`; see static analysis report. |
| Are dates strictly validated? | Needs improvement | `validDateTime()` performs only a shallow format check. |
| Are schema migrations checked for failure? | Needs improvement | WorkflowService constructor ignores migration `execute()` return values. |
| Are submission contents actually stored? | No | Only a path and timestamp are persisted; do not describe this as file-content upload. |
| Are mark changes and moderation decisions auditable? | Implemented in workflow logic; needs full end-to-end manual verification | Verify actor, timestamp, old/new values and reason in system tests. |
| Are tests registered in CTest? | Yes in this revision | Three GoogleTest executables are registered in CMake. |
| Does CI build and run tests on pushes/PRs? | Configured | `.github/workflows/ci.yml`; successful hosted run still required. |
| Are generated artefacts excluded from version control? | Review required | Check `.gitignore` against local build, gcov, database and IDE artefacts before push. |

## Changes made as a result of this review

1. Kept all three test executables registered in `CMakeLists.txt` and linked the GCC coverage runtime to each executable.
2. Added a CI static-analysis step for cppcheck and retained build/test/coverage steps.
3. Added a reproducible metrics script and documentation describing its counting method and limitations.
4. Added explicit Git branching, commit, pull-request and annotated-tag procedures.
5. Recorded unresolved risks instead of claiming they were fixed without implementation and retesting.

## Outstanding corrective actions

| ID | Priority | Action required | Closure evidence |
|---|---|---|---|
| QA-01 | High | Strictly validate ISO calendar dates/timestamps in workflow inputs | Unit tests for valid, invalid and boundary dates; passing CTest |
| QA-02 | High | Propagate schema migration failures from WorkflowService setup | Migration failure test and safe startup failure |
| QA-03 | Medium | Distinguish database errors from not-found/unauthorised results | Updated service result contract and tests |
| QA-04 | High | Implement real submission content storage or clearly constrain/document path-based submission | Integration test verifies content retrieval and persistence, or approved scope decision |
| QA-05 | High | Complete user-facing flows for Administrator, Lecturer, HoD and Student | Role-by-role acceptance test evidence |
| QA-06 | Medium | Run cppcheck on the final Windows/CI source and resolve findings | Saved output and clean rerun or justified suppressions |
| QA-07 | Medium | Verify Git remote, branch rules, meaningful commits and release tags | Repository URL, PR/commit/tag evidence |

## Review conclusion

The code has useful separation, parameterised SQL, password hashing and automated test suites. The review also identified unresolved validation, migration-error-handling and application-completeness risks. Phase 6 self-review is documented, but the QA phase must not be declared fully closed until high-priority actions and external evidence are addressed.
