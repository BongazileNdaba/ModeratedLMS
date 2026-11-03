# Moderated LMS â€” Release Notes v1.0

**Release designation:** v1.0 (initial baseline target)
**Product:** Moderated Learning Management System
**Technology:** C++17, SQLite, libsodium
**Status:** Prototype / controlled demonstration baseline â€” not production approved

## 1. Release summary

Version 1.0 establishes the initial backend foundation for the Moderated LMS case study. It provides persistent SQLite storage, account authentication using salted Argon2id password hashes, and core assessment workflow services. This release is suitable for controlled development, testing and demonstration of implemented backend capabilities. It is not a complete interactive school-wide LMS.

## 2. Included capabilities

- SQLite database initialisation and schema for departments, users, courses, enrolments, assignments, extensions, submissions, marks, moderation and audit records.
- User account creation and lookup through a repository layer.
- Password hashing and verification using libsodium Argon2id with random salts.
- Authentication checks for invalid credentials and inactive accounts.
- Workflow service operations for assignment creation, individual extensions, whole-class extensions, versioned submission records, marking, moderation and controlled mark release.
- Late penalty rules based on the effective deadline: no penalty on deadline date; 10% of maximum mark per calendar day for up to three days; after three days, submission is recorded but mark is zero.
- GoogleTest automated suites for workflow, authentication and password hashing, with CTest registration in the project configuration.
- CI workflow configuration for build and automated test execution.

## 3. Known limitations at this release

- `main.cpp` is a demonstration entry point for account creation and authentication, not a complete role-based interactive application.
- Complete Administrator, Lecturer, HoD and Student menus/views are not implemented.
- Student-facing assignment lists and released-mark/feedback views are not implemented.
- Submission handling records a file path and timestamp; it does not copy, inspect or validate the file contents.
- No network client/server protocol is implemented. The current program is not a multi-workstation LMS server.
- Performance and concurrency for 800 students have not been demonstrated by a school-hardware load test.
- Manual system and acceptance testing must be completed before operational use.
- The release tag `v1.0` must be created in the actual Git repository after release approval; a ZIP archive alone does not prove a Git tag exists.

## 4. Installation and verification

See `PHASE7_INSTALLATION_GUIDE.md`. The release should be built and verified using the exact commands and dependency versions recorded there. Attach actual build and test logs to the release record. Do not state that a build, test or CI run passed unless its output is retained.

## 5. Upgrade and data safety

Back up the SQLite database before replacing binaries or applying schema changes. Test updates on a copy of the database first. Preserve the previous executable and database backup so the technician can roll back if the new build fails.

## 6. Release approval record

| Field | Value |
|---|---|
| Release version | v1.0 |
| Release date | To be entered after approval |
| Git commit | To be recorded from the actual repository |
| Git tag | `v1.0` â€” pending creation/verification |
| Build result | Must be recorded from release build |
| Automated test result | Must be recorded from release test run |
| Approved by | School/project supervisor â€” pending |
| Deployment decision | Controlled prototype only; production deployment not approved |

## 7. Suggested Git tagging procedure

After the final v1.0 source has been reviewed, built and tested in the real repository:

```bash
git status
git add .
git commit -m "Prepare Moderated LMS v1.0 release"
git tag -a v1.0 -m "Moderated LMS version 1.0"
git push origin main
git push origin v1.0
```

If the project uses a different default branch, replace `main` with that branch. Do not create the tag until the release is approved and the working tree is in the intended state.
