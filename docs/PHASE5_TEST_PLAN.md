# Phase 5 â€“ Verification and Validation Test Plan

## 1. Purpose and scope

This plan verifies the Moderated LMS against its requirements and use cases. It covers authentication, assignment setup, individual and whole-class extensions, student submissions/resubmissions, late-penalty calculation, marking, moderation and release. Tests are grouped according to the V-Model levels.

## 2. V-Model mapping

| V-Model development level | Verification activity | Artefact verified | Entry criteria | Exit criteria |
|---|---|---|---|---|
| Requirements analysis | Acceptance-test design | Phase 2 functional and non-functional requirements; assignment brief | Requirements are baselined and understandable | Each testable requirement has an acceptance scenario or documented reason it cannot be tested yet |
| System design | System-test design | Phase 3 architecture, database design and use cases | System design and interfaces are available | Normal and exception flows are represented in system tests |
| Architectural/high-level design | Integration-test design | MVC/service/repository interfaces and database schema | Components and contracts are defined | Each critical component interaction has an integration test |
| Module/detailed design | Unit-test design | Domain classes, services and business rules | Unit interfaces are stable | Boundary and equivalence classes are identified for business rules |
| Coding | Unit, integration, system and acceptance execution | Implemented source code and executable | Build succeeds and test environment is prepared | Test results recorded, defects triaged and coverage reported |

## 3. Test strategy and techniques

- **Unit tests:** test individual business rules and validation boundaries.
- **Integration tests:** test service/repository/SQLite interactions, including transaction-sensitive operations.
- **System tests:** execute complete user workflows across student, lecturer and HoD roles.
- **Acceptance tests:** evaluate the workflows against school-facing requirements and expected outcomes.
- **Equivalence partitioning:** valid/invalid roles, enrolled/not-enrolled students, valid/invalid marks, and submissions on-time/late.
- **Boundary value analysis:** deadline day, one day late, three days late and four days late; zero, maximum and above-maximum marks.

## 4. Core boundary-value cases

Assume maximum mark = 100 and the effective deadline is 2026-10-10. For raw mark 80:

| Submitted date | Late days | Expected penalty | Expected final mark |
|---|---:|---:|---:|
| 2026-10-10 | 0 | 0 | 80 |
| 2026-10-11 | 1 | 10 | 70 |
| 2026-10-12 | 2 | 20 | 60 |
| 2026-10-13 | 3 | 30 | 50 |
| 2026-10-14 | 4 | 100% of maximum | 0 |

The >3-day case is accepted and retained, but receives zero. Extension cases must use the revised effective deadline rather than the original deadline.

## 5. Test cases

| ID | Level | Scenario / input | Expected result |
|---|---|---|---|
| UT-01 | Unit | Create assignment with non-positive maximum mark | Rejected |
| UT-02 | Unit | Raw mark is negative, non-finite or above maximum | Rejected |
| UT-03 | Unit | Deadline day submission | No late penalty |
| UT-04 | Unit | Submission 1, 2 or 3 calendar days late | 10% of maximum deducted per day |
| UT-05 | Unit | Submission more than 3 calendar days late | Accepted; final mark zero |
| IT-01 | Integration | Student submits while enrolled | New version stored as current |
| IT-02 | Integration | Student resubmits | Prior version retained; new version becomes current |
| IT-03 | Integration | Unenrolled student submits | Rejected |
| IT-04 | Integration | Lecturer grants individual extension | Revised effective deadline stored for that student |
| IT-05 | Integration | Lecturer grants whole-class extension | Revised effective deadline applies to enrolled students in the course |
| IT-06 | Integration | Lecturer marks submission | Mark, penalty, final mark and audit record stored |
| IT-07 | Integration | HoD approves mark | Status becomes Approved and moderation record is stored |
| IT-08 | Integration | HoD rejects mark | ChangesRequested record stored; mark returns to Draft |
| IT-09 | Integration | Release a Draft/AwaitingModeration mark | Rejected |
| IT-10 | Integration | Release an Approved mark | Status becomes Released |
| ST-01 | System | Student login, open assignment, submit and resubmit | Both versions retained; latest is current |
| ST-02 | System | Lecturer grants extension, marks and provides feedback | Correct effective deadline and mark calculation |
| ST-03 | System | HoD reviews and releases a mark | Student can see the released result only after release |
| AT-01 | Acceptance | Student attempts to access another student's submission | Access denied by the application/controller layer |
| AT-02 | Acceptance | Student requests unreleased mark | Result is not disclosed |
| AT-03 | Acceptance | Lecturer tries to mark another lecturer's assignment | Operation rejected |

## 6. Entry and exit criteria

**Entry:** source code is available; dependencies are installed; database schema initializes; test database is isolated; test data and user roles are prepared.

**Exit:** all critical tests pass; failures are documented; no unresolved critical/high-severity defect remains without an explicit accepted waiver; CI build and tests pass; actual business-logic line coverage is measured and reported. The assignment target is at least 70%; if not achieved, identify uncovered code and add tests before final submission.

## 7. Defect handling

Every defect entry records an ID, severity, observed behaviour, cause, correction, retest result and status. Do not mark a defect fixed until the corresponding retest passes.
