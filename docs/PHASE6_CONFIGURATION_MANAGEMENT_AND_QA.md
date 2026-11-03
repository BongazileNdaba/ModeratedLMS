# CS4433 Phase 6 â€” Configuration Management and Quality Assurance

**Project:** Moderated Learning Management System (LMS)
**Release represented by this source snapshot:** v1.1 (Phase 8 change request implementation)
**Status:** Evidence pack prepared; hosted-repository, CI-run, and static-analyser execution evidence must be completed against the owner's GitHub repository and Windows toolchain.

## 1. Purpose and assessment mapping

Phase 6 is worth 6 marks: 3 for configuration management and 3 for quality assurance. This document records the controls and evidence expected by the assignment brief.

| Brief requirement | Evidence in this project | Status |
|---|---|---|
| Git repository and documented branching strategy | `docs/PHASE6_GIT_WORKFLOW.md` | Strategy documented; remote URL not present in uploaded ZIP |
| Meaningful commits | Commit convention and suggested commit sequence in `docs/PHASE6_GIT_WORKFLOW.md` | Guidance supplied; historical commits cannot be reconstructed from a ZIP |
| Tagged releases | Release/tagging procedure in `docs/PHASE6_GIT_WORKFLOW.md` | Procedure supplied; verify actual tags in hosted repository |
| CI builds project and runs tests on every push | `.github/workflows/ci.yml` | Workflow configuration present; successful hosted run must be evidenced by GitHub Actions |
| Static analyser run and findings addressed | CI cppcheck step and `docs/PHASE6_STATIC_ANALYSIS_REPORT.md` | Automated run configured; local execution blocked where cppcheck is not installed |
| Code metrics | `scripts/collect_metrics.py` and `docs/PHASE6_CODE_METRICS.md` | Reproducible source metrics script supplied |
| Self code review and recorded changes | `docs/PHASE6_CODE_REVIEW_RECORD.md` | Review record supplied; unresolved findings explicitly tracked |

## 2. Configuration management controls

The source tree is the configuration item. The following are version-controlled: C++ source and headers, CMake configuration, tests, test data, CI workflow, and project documentation. Generated build directories, binaries, database files, coverage counters/reports, and IDE-local files must not be committed unless a specific release artefact is intentionally archived.

The repository should use `main` as the protected integration branch. Work is carried out on short-lived `feature/<topic>` or `fix/<topic>` branches and merged by pull request after CI passes and the change is reviewed. The detailed process, commit format, release tags and recovery rules are in `PHASE6_GIT_WORKFLOW.md`.

The uploaded ZIP does not contain `.git` metadata or a remote URL. Therefore this report does not invent a repository link, commit history, pull-request history, or release-tag evidence. Add the actual hosted repository URL and screenshots/exported run links before submission.

## 3. Continuous integration

The GitHub Actions workflow runs for pushes and pull requests targeting `main` or `master`. It checks out the source, installs CMake, GCC, SQLite, libsodium, GoogleTest build dependencies, gcovr and cppcheck; configures CMake with tests enabled; builds; runs CTest; runs cppcheck; generates service coverage; and uploads the coverage XML as an artifact.

A workflow file is configuration, not proof of a successful run. Record the actual Actions run URL, commit SHA, date, job result, CTest summary, cppcheck result and coverage artifact after pushing this revision to the hosted repository.

## 4. Quality assurance approach

Quality checks used or configured:

- Compiler diagnostics: `-Wall -Wextra -Wpedantic` on the core library.
- Static analysis: cppcheck with warning, performance and portability checks; see `PHASE6_STATIC_ANALYSIS_REPORT.md`.
- Automated tests: GoogleTest suites registered with CTest.
- Coverage: gcovr filters the service implementation files; report the actual measured result, not a target or estimate.
- Manual review: checklist and review findings in `PHASE6_CODE_REVIEW_RECORD.md`.
- Reproducible metrics: `python scripts/collect_metrics.py`.

## 5. Acceptance criteria for Phase 6 evidence

Phase 6 can be marked complete only when all of the following are true:

1. The real hosted Git repository URL is recorded and accessible to the assessor.
2. Branch protection / pull-request workflow is demonstrated, or the documented equivalent is evidenced.
3. Meaningful commits and release tags exist in the actual repository; do not create retrospective fake history.
4. At least one successful CI run for the submitted commit shows build and tests passing.
5. cppcheck output is attached and every finding is either fixed or documented with a justified suppression/risk decision.
6. Metrics are generated from the final submitted source snapshot.
7. The self-review checklist records concrete changes and remaining accepted risks.

## 6. Limitations and evidence integrity

This archive is a source snapshot, not a clone of the hosted Git repository. It cannot prove earlier commits, branch merges, tags, or Actions runs. The current execution environment did not have cppcheck installed at audit time, so no cppcheck-clean claim is made. The CI workflow is configured to run cppcheck after the repository is pushed. Any outstanding source-review risks are listed rather than silently treated as fixed.
