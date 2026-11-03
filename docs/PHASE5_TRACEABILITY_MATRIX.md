# Phase 5 â€“ Requirements-to-Test Traceability Matrix

This matrix connects the main agreed LMS behaviours to implementation and verification evidence. Confirm exact requirement identifiers against the final Phase 2 SRS before submission; descriptive labels are used here where the precise SRS numbering is not available in this source package.

| Requirement / behaviour | Use case / workflow | Implementation element | Automated test(s) | Manual test(s) |
|---|---|---|---|---|
| Lecturer creates valid assignment | Create assignment | `WorkflowService::createAssignment`; `assignments` | `RejectsInvalidAssignmentDetails` | SYS-01 |
| Student submits only when active and enrolled | Submit assignment | `WorkflowService::submit`; `users`, `enrollments`, `submissions` | `ResubmissionPreservesVersionsAndOnlyLatestIsCurrent` (plus role checks) | SYS-02 to SYS-08 |
| Deadline day is not late | Submit / mark | `calendarDaysLate`, `recordMark` | `SubmissionOnDeadlineIsNotLate` | SYS-02 |
| Late penalty is 10% of max mark per day for days 1â€“3 | Mark submission | `WorkflowService::recordMark`; `marks` | `OneCalendarDayLateDeductsTenPercentOfMaximum`, `ThreeCalendarDaysLateDeductsThirtyPercentOfMaximum` | SYS-03, SYS-04 |
| More than 3 days late is recorded but final mark is zero | Mark late submission | `submit`, `recordMark`; `submissions`, `marks` | `MoreThanThreeDaysLateIsAcceptedButReceivesZero` | SYS-05 |
| Resubmissions preserve previous versions | Resubmit assignment | `WorkflowService::submit`; `submissions` | `ResubmissionPreservesVersionsAndOnlyLatestIsCurrent`, `LateResubmissionBecomesCurrentAndUsesItsOwnSubmissionDate` | SYS-06 |
| Lecturer may grant individual extension only to enrolled student | Grant individual extension | `grantIndividualExtension`; `individual_extensions` | `IndividualExtensionChangesOnlyThatStudentsEffectiveDeadline`, `RejectsInvalidExtensionAndUnenrolledStudent` | SYS-07, SYS-09 |
| Lecturer may grant extension to whole class | Grant class extension (CR1) | `grantClassExtension`; `whole_class_extensions` | `WholeClassExtensionChangesDeadlineForEveryEnrolledStudent` | SYS-08, SYS-15 |
| Mark cannot be negative or exceed maximum | Record mark | `recordMark`; `marks` constraints | `RejectsUnauthorisedExtensionAndInvalidMark`, `RejectsNonFiniteMarks`, `AcceptsZeroAndMaximumMarkBoundaries` | SYS-10 |
| Mark correction is auditable | Correct mark | `recordMark`; `mark_audit_log` | `MarkCorrectionCreatesAuditRecordWithOldAndNewValues` | SYS-11 |
| HoD can request changes or approve | Moderate mark | `moderateMark`; `moderation_records` | `HoDCanRequestChangesButCannotReleaseUnapprovedMark`, `ApprovedMarkCanBeReleasedByHoD` | SYS-12, SYS-13 |
| Only HoD can moderate/release; only assignment lecturer can mark | Role-protected actions | `hasRole`, `recordMark`, `moderateMark`, `releaseMark` | `RejectsUnauthorisedExtensionAndInvalidMark`, `NonHoDCannotModerateOrReleaseMarks` | SYS-14 |
| Students see class revised deadline and released marks | View assignment list / released marks | Student-facing view/controller (not demonstrated by the current workflow service) | No UI-level automated test in this package | SYS-15, SYS-16 (verify separately) |

## Traceability limitation

The current project archive contains service and persistence code but does not contain a complete interactive role-based student/lecturer/HoD UI. Therefore SYS-15 and SYS-16 are explicit acceptance checks, not claims that the current code already demonstrates those screens. Map the descriptive requirements above to the exact Phase 2 SRS IDs before final submission.
