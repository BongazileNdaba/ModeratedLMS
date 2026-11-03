# Phase 7 â€” Deployment Package and Deliverable Checklist

**Assignment:** CS4433 Software Engineering Individual Assignment
**System:** Moderated LMS
**Phase value:** 4 marks

## 1. Mapping to the assignment brief

| Assignment requirement | Project artefact | Completion / evidence status |
|---|---|---|
| UML Deployment Diagram | `PHASE7_DEPLOYMENT_DIAGRAM.png`, `.svg`, `.dot` | Diagram prepared; explicitly distinguishes deployed prototype components from intended client connections. |
| Installation guide for school technician | `PHASE7_INSTALLATION_GUIDE.md` | Prepared with dependencies, Windows/MSYS2 setup, build, test, launch, backup, update, troubleshooting and acceptance checks. |
| User manual for Administrator | `PHASE7_USER_MANUAL_ADMINISTRATOR.md` | Prepared; interactive screens are identified as not yet implemented. |
| User manual for Lecturer | `PHASE7_USER_MANUAL_LECTURER.md` | Prepared; workflow and late penalty rules explained; UI limitations flagged. |
| User manual for HoD | `PHASE7_USER_MANUAL_HOD.md` | Prepared; moderation and release procedure explained; UI limitations flagged. |
| User manual for Student | `PHASE7_USER_MANUAL_STUDENT.md` | Prepared; submission/resubmission and released-results workflow explained; UI limitations flagged. |
| Release notes | `PHASE7_RELEASE_NOTES_v1.0.md` | Prepared; accurately describes v1.0 prototype scope and known limitations. |
| Tag release as v1.0 | Actual Git tag `v1.0` | Pending; must be created and pushed in the user's real Git repository after approval. |

## 2. Deployment architecture explanation

The intended deployment is an offline school environment with a school computer hosting the C++ application and SQLite database. The application uses libsodium for password hashing. Internet connectivity is not required for normal runtime once the dependencies and binaries are installed. It may be required during the first test-enabled CMake configuration to fetch GoogleTest.

The current archive is a backend/service prototype. The diagram therefore shows workstation-to-application connections as dashed target links, not as an implemented network service. No multi-client protocol or complete role-specific UI is present in this archive.

## 3. Release boundary

The v1.0 notes describe what is actually present: database schema, authentication, password hashing and workflow services. They also identify missing production capabilities, including full role menus, student-facing assignment/result views, actual file-content handling, network access and 800-user performance evidence. These limitations must not be omitted from a viva or deployment decision.

## 4. Final Phase 7 acceptance checklist

- [x] Deployment diagram source and rendered formats supplied.
- [x] Technician installation guide supplied.
- [x] Administrator user manual supplied.
- [x] Lecturer user manual supplied.
- [x] HoD user manual supplied.
- [x] Student user manual supplied.
- [x] v1.0 release notes supplied.
- [ ] Release build and test logs captured from the actual release commit.
- [ ] Actual Git tag `v1.0` created and pushed; repository URL recorded.
- [ ] Supervisor/user acceptance and production deployment approval obtained.
- [ ] Manuals updated to match the final implemented user interface.

**Phase status:** Documentation deliverables prepared. Release tagging and operational validation remain outstanding. Because the application does not yet contain all role interfaces and production capabilities, Phase 7 must not be represented as a fully operational production deployment.
