# User Manual â€” Head of Department (HoD)

**Role:** Head of Department
**System:** Moderated LMS
**Release target:** v1.0

## 1. Role and responsibilities

The Head of Department reviews marks for courses in their department, checks assessment decisions and fairness, approves marks or requests changes, and controls release to students. Students must not see marks before moderation and approval.

## 2. Intended operating procedures

The supplied archive implements moderation and release operations in the workflow service, but does not yet provide a complete interactive HoD menu. The following describes the target workflow.

### 2.1 Review marks awaiting moderation
1. Sign in with your HoD account.
2. Open **Department moderation**.
3. Select the course and assignment.
4. Review each submission, raw mark, calculated late penalty, final mark and lecturer feedback.
5. Check that the effective deadline and any approved extension have been applied correctly.
6. Identify missing feedback, unsupported mark changes or inconsistent decisions.

### 2.2 Approve a mark
1. Select the mark after completing the review.
2. Choose **Approve**.
3. Add a moderation comment if required by school procedure.
4. Confirm the status changes to approved.
5. Use the authorised release operation only after approval.

### 2.3 Request changes
1. Select **Request changes**.
2. Enter a clear explanation of the issue (for example, arithmetic error, missing feedback or inconsistent penalty).
3. Submit the request to the lecturer.
4. Re-review the corrected mark when it is resubmitted.

### 2.4 Release approved marks
1. Confirm the mark has approved moderation status.
2. Select the authorised release action.
3. Confirm the release operation.
4. Verify the released status and audit record.

The backend workflow is designed to reject release of a mark that has not been approved. The final interface must expose this restriction clearly.

## 3. Audit and confidentiality

- Review mark history and reasons for changes when investigating a dispute.
- Do not edit marks directly in SQLite.
- Restrict access to assessment information to authorised departmental personnel.
- Keep moderation comments professional and factual.
- Follow the school's retention and data-protection policy.

## 4. Common problems

- **Mark cannot be released:** check that moderation approval has been recorded.
- **Mark appears incorrect:** inspect the raw mark, maximum mark, effective deadline, late-day calculation and audit history; request correction rather than changing the database directly.
- **Assignment is outside your department:** do not moderate it; refer it to the correct HoD or system administrator.

## 5. Current release limitation

The archive contains backend moderation and release logic, but no complete interactive HoD dashboard or departmental mark-list screen. The manual describes the intended role workflow; user-interface availability must be verified in a future complete application before this manual is issued as an operational guide.
