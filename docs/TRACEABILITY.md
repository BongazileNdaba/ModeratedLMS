# Requirements Traceability Matrix (starter)

| Requirement | Implementation location | Verification status |
|---|---|---|
| C++17 offline application | `CMakeLists.txt`, `main.cpp` | Build must be confirmed on target Windows PC |
| SQLite persistence and relational schema | `src/repositories/Database.cpp` | Schema initializes; existing user's earlier build verified before latest changes |
| Secure password hashing | `src/services/PasswordHasher.cpp` | 12 earlier authentication checks passed before console workflow changes |
| Account roles | `include/domain/User.h`, `UserRepository`, role menus in `main.cpp` | Role menus implemented; authorization hardening still required |
| Account management | Administrator menu in `main.cpp` | Manual test required |
| Departments, courses and enrolment | Administrator menu in `main.cpp` | Manual test required |
| Assignments and deadlines | Lecturer menu in `main.cpp` | Manual test required |
| Individual extensions | `individual_extensions`, Lecturer menu | Manual test required |
| Whole-class extensions (v1.1) | `whole_class_extensions`, Lecturer menu | Manual test required |
| Student submission/resubmission versions | `submissions`, Student menu | Manual test required; records file path only |
| Late penalty: calendar days, 10% max mark/day for up to 3 days; >3 days zero | Marking branch in `main.cpp` | Needs edge-case tests around midnight and extensions |
| Raw mark, penalty and final mark | `marks` columns; Lecturer marking | Manual test required |
| HoD moderation and release | `moderation_records`, HoD menu | Manual test required |
| Mark audit trail | `mark_audit_log`, Lecturer marking | Partial: current audit insert uses NULL old value; improve before final submission |
| Transactional resubmission, marking+audit, moderation | `main.cpp` transaction blocks | Manual failure-path tests required |
| Automated tests and coverage | `tests/` | Existing starter harness had 12 checks; comprehensive workflow tests/coverage still outstanding |
| CI pipeline | `.github/workflows/ci.yml` | CI configured; remote run not verified |
| Deployment instructions | `README.md` | Written; validate against target machine |


## Phase 8 CR1 traceability additions

| Change requirement | Design / implementation | Regression test |
|---|---|---|
| Lecturer grants class-wide extension | `whole_class_extensions`; `WorkflowService::grantClassExtension` | `WholeClassExtensionChangesDeadlineForEveryEnrolledStudent` |
| Student assignment list shows effective class deadline | `StudentAssignment`; `WorkflowService::listAssignmentsForStudent` | `StudentAssignmentListDisplaysWholeClassExtendedDeadline` |
| Individual extension affects only its student | `effectiveDeadline`; student assignment-list query | `StudentAssignmentListShowsIndividualExtensionOnlyToThatStudent` |
| Only active students may retrieve their assignment list | `hasRole`; `listAssignmentsForStudent` | `StudentAssignmentListRequiresActiveStudentRole` |
