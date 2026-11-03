# Moderated LMS (C++17)

An offline Learning Management System prototype using C++17, SQLite and libsodium. The application separates persistence (repositories), business rules (services), and domain models. MVC controllers/views can be added around these services without moving business rules into the UI.

## Implemented foundation

- SQLite schema for departments, users, courses, enrolments, assignments, extensions, submissions, marks, moderation and audit records.
- Argon2id password hashing through libsodium.
- User account repository and authentication service.
- Workflow service for assignment creation, individual and whole-class extensions, versioned submissions, late penalties, lecturer marking, HoD moderation and mark release.
- GoogleTest automated integration tests and GitHub Actions CI configuration.

## Late-submission policy

- The deadline date itself is not late.
- Each calendar day late deducts 10% of the assignment's maximum mark.
- Up to three late days are accepted with the corresponding penalty.
- More than three days late is accepted and recorded, but the final mark is zero.
- The lecturer-approved effective deadline (individual or whole-class extension) is used for the calculation.

## Requirements

- CMake 3.20 or later
- C++17 compiler (GCC/MinGW or MSVC)
- SQLite development library
- libsodium development library
- Internet access on the first test-enabled CMake configure, because GoogleTest is fetched by CMake FetchContent

### Windows / MSYS2 UCRT64

Install dependencies in the MSYS2 UCRT64 terminal:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-sqlite3 mingw-w64-ucrt-x86_64-libsodium
```

Open PowerShell or the VS Code terminal where CMake and the UCRT64 compiler are on PATH, then run:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
.\build\ModeratedLMS.exe
```

The first configure downloads GoogleTest. If the machine is offline, configure with `-DBUILD_TESTING=OFF` to build the application only; automated tests will not be available in that build.

## Workflow service API

`include/services/WorkflowService.h` exposes the core use cases:

- `createAssignment(...)`
- `grantIndividualExtension(...)`
- `grantClassExtension(...)`
- `submit(...)` — stores each submission as a new version and marks only the newest version current
- `recordMark(...)` — validates the raw mark, calculates the late penalty and records an audit entry
- `moderateMark(...)` — HoD approval or request for changes
- `releaseMark(...)` — only an approved mark can be released

The service expects valid existing user/course/enrolment records. A controller or console/UI layer should authenticate the current user and pass that user's database ID to the service. The service also checks the required active role and assignment lecturer ownership for protected operations.

## Testing and coverage

GoogleTest suites are in:

- `tests/WorkflowServiceTests.cpp`
- `tests/AuthenticationServiceTests.cpp`
- `tests/PasswordHasherTests.cpp`

All three suites are registered with CTest. Run `ctest --test-dir build --output-on-failure` after configuring and building. GCC builds instrument `lms_core` for coverage. The GitHub Actions workflow builds, runs CTest and uploads a service coverage XML report. A workflow file is configuration, not evidence that a remote CI run has already passed; inspect the Actions run and artifact before reporting CI success.

See `docs/PHASE5_EXECUTION_STATUS.md` for the distinction between user-reported local test results, measured service coverage and manual tests that remain pending.

## Database note

The program creates missing tables on startup. `WorkflowService` performs additive migrations for the mark calculation fields and whole-class extension table. Back up any database before testing schema changes.


## Current implementation boundary

This archive is a backend/service prototype, not yet a complete interactive LMS application. `main.cpp` currently demonstrates account creation and authentication rather than providing complete Administrator, Lecturer, HoD and Student menus/views. `WorkflowService::submit` stores a submitted file path and timestamp; it does not copy or validate the file contents. Student-facing assignment lists and released-mark views are not implemented in this archive. These are important completion items, not features to claim as finished.

## Phase 6 — Configuration Management and Quality Assurance

Phase 6 evidence is documented in:

- `docs/PHASE6_CONFIGURATION_MANAGEMENT_AND_QA.md` — assignment mapping and completion status.
- `docs/PHASE6_GIT_WORKFLOW.md` — branching, pull requests, commit messages and release tagging.
- `docs/PHASE6_STATIC_ANALYSIS_REPORT.md` — cppcheck configuration, execution status and findings register.
- `docs/PHASE6_CODE_METRICS.md` — source-size and approximate complexity measurements.
- `docs/PHASE6_CODE_REVIEW_RECORD.md` — self-review checklist, changes and outstanding actions.
- `scripts/collect_metrics.py` — reproducible metric collection script.

The GitHub Actions workflow now includes build, CTest, cppcheck, source metrics and coverage evidence. A hosted repository URL and successful Actions run must still be captured from the actual GitHub repository; the ZIP itself does not contain `.git` metadata. Static-analysis findings must be updated from the first real cppcheck run; no zero-finding claim is made without that output.

## Phase 7 — Deployment

Phase 7 deployment documentation is in `docs/`:

- `PHASE7_DEPLOYMENT_AND_USER_DOCUMENTATION.md` — assignment mapping and completion checklist.
- `PHASE7_DEPLOYMENT_DIAGRAM.png` / `.svg` / `.dot` — UML deployment view in rendered and editable source formats.
- `PHASE7_INSTALLATION_GUIDE.md` — Windows/MSYS2 installation, build, launch, backup, update and troubleshooting instructions.
- `PHASE7_USER_MANUAL_ADMINISTRATOR.md` — Administrator role guide.
- `PHASE7_USER_MANUAL_LECTURER.md` — Lecturer role guide.
- `PHASE7_USER_MANUAL_HOD.md` — Head of Department role guide.
- `PHASE7_USER_MANUAL_STUDENT.md` — Student role guide.
- `PHASE7_RELEASE_NOTES_v1.0.md` — v1.0 scope, limitations and release-tag procedure.

**Release status:** Documentation has been prepared, but the actual Git tag `v1.0`, release build/test evidence and production approval must be completed in the real repository. The current archive remains a backend/service prototype, not a production-ready multi-user LMS. The manuals explicitly flag role interfaces and other capabilities that are not implemented yet.


## Phase 8 — Maintenance and Change Request CR1

Phase 8 candidate documentation and implementation are in `docs/PHASE8_CHANGE_REQUEST_CR1.md`, `docs/PHASE8_REGRESSION_STATUS.md` and `docs/PHASE8_RELEASE_NOTES_v1.1.md`. `WorkflowService` now exposes a student assignment-list query containing original/effective deadlines and an extension indicator. Automated regression tests have been added. The tests still need to be built and run on the configured Windows/MSYS2 machine. The interactive student assignment-list screen remains outstanding, and v1.1 must not be tagged until verification is complete.

## Interactive application (updated console interface)

The `ModeratedLMS` executable is the interactive application. It is no longer an authentication-test demonstration. On the first run it asks you to create the initial Administrator account; afterwards, users sign in with their own credentials and see the menu for their role.

### Role workflows

- **Administrator:** create accounts, activate/deactivate accounts, create departments and courses, enrol students, and inspect account/course records.
- **Lecturer:** create assignments, grant individual or whole-class deadline extensions, inspect current submissions, and record marks with feedback and an audit reason.
- **Head of Department:** inspect marks awaiting moderation, approve marks or request changes, and separately release approved marks to students.
- **Student:** view assignments and their effective deadlines, submit/resubmit a local file, inspect submission history, and view released marks and feedback.

The role menus call `WorkflowService` for assignment, extension, submission, marking, moderation and release operations. Database-level role and ownership checks remain in the service layer; hiding an option in a menu is not treated as an authorization check.

### Date and submission rules

Enter deadlines in `YYYY-MM-DD HH:MM:SS` format. The application records the local time when a student submits. A student must select an existing local file. The current implementation stores the path and submission metadata in SQLite; it does **not** copy the file into managed LMS storage or transmit it over a network. Keep submitted files at the recorded path.

The workflow service applies the project's late policy: the deadline date itself is on time; each calendar day late up to three days incurs 10% of the maximum mark; more than three calendar days late is accepted and recorded but receives zero. Individual and class-wide extensions affect the effective deadline shown to the student.

### Build and run (Windows / MSYS2 UCRT64)

Open the UCRT64 terminal, change to the project folder, and run:

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
./build/ModeratedLMS.exe
```

If you use an existing build directory, rebuild after pulling or replacing source files. If coverage reports old `.gcda` checksum/profile errors, remove the build directory and configure a clean build before collecting coverage again. Keep the `moderated_lms.db` file backed up; it contains the LMS records.

### Verification status

The previously supplied project snapshot had 36 automated tests passing before this interactive `main.cpp` integration. Re-run the build and tests against this updated package before treating the integrated executable as verified. The current development environment used to prepare this package does not have the MSYS2/libsodium toolchain, so Windows compilation, live role walkthroughs, file-path behaviour and any 800-student performance claim must be verified on the target machine. The automated test result from the earlier snapshot is not being represented as a fresh test of this updated interface.
