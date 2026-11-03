# Phase 5 â€“ Manual System and Acceptance Test Cases

## Test environment and test data

- Course: SCI101 â€“ Introduction to Computer Programming
- Assignment: Programming Fundamentals Assignment
- Maximum mark: 100
- Base deadline: 10 November 2026, 23:59
- Roles: one Administrator, one Lecturer, students Aâ€“G, and one HoD
- Use a dedicated test database. Do not use real student accounts or personal submissions.
- The sample `.cpp` files are fictional demonstration data. In the current backend, submit the corresponding file path; the service records the path and timestamp but does not store file bytes.

## Expected late-penalty calculations

Penalty per late day = 10% Ã— maximum mark = 0.10 Ã— 100 = 10 marks.

| Student | Submitted | Days late | Raw mark | Penalty | Expected final mark |
|---|---|---:|---:|---:|---:|
| A | 10 Nov 2026 | 0 | 80 | 0 | 80 |
| B | 11 Nov 2026 | 1 | 80 | 10 | 70 |
| C | 13 Nov 2026 | 3 | 80 | 30 | 50 |
| D | 14 Nov 2026 | 4 | 95 | 100 | 0 |
| E | 10 Nov 2026, then resubmits | 0 | 75 | 0 | 75 on current version |
| F | 11 Nov 2026, individual extension to 12 Nov | 0 | 80 | 0 | 80 |
| G | 11 Nov 2026, class extension to 12 Nov | 0 | 70 | 0 | 70 |

The test dates are calendar dates. The deadline day itself is not late. A submission more than three calendar days late is accepted and recorded but receives zero.

## Manual test cases

| ID | Level | Scenario / steps | Expected result | Actual result / evidence |
|---|---|---|---|---|
| SYS-01 | System | Lecturer creates the assignment with title, description, deadline and maximum mark 100. | Assignment is saved and visible to enrolled students. | To be completed during execution |
| SYS-02 | System | Student A submits `Student_A_OnTime.cpp` on 10 Nov. Lecturer marks raw 80. | Submission accepted; penalty 0; final mark 80. | To be completed during execution |
| SYS-03 | System | Student B submits `Student_B_OneDayLate.cpp` on 11 Nov. Lecturer marks raw 80. | Submission accepted; penalty 10; final mark 70. | To be completed during execution |
| SYS-04 | System | Student C submits `Student_C_ThreeDaysLate.cpp` on 13 Nov. Lecturer marks raw 80. | Submission accepted; penalty 30; final mark 50. | To be completed during execution |
| SYS-05 | System | Student D submits `Student_D_FourDaysLate.cpp` on 14 Nov. Lecturer marks raw 95. | Submission is retained and identified as late-zero; final mark 0. | To be completed during execution |
| SYS-06 | System | Student E submits version 1, then submits version 2. Inspect submission rows. | Both versions remain in history; version 2 is current; version 1 is not current. | To be completed during execution |
| SYS-07 | System | Lecturer grants Student F an individual extension to 12 Nov; F submits 11 Nov. Student B has no extension and submits 11 Nov. | F has no penalty; B receives one-day penalty. | To be completed during execution |
| SYS-08 | System / regression | Lecturer grants a whole-class extension to 12 Nov; two enrolled students submit 11 Nov. | Both students receive no late penalty. | To be completed during execution |
| SYS-09 | System | Lecturer attempts to grant an extension with an earlier deadline or empty reason. | Request is rejected and no invalid extension is stored. | To be completed during execution |
| SYS-10 | System | Lecturer attempts to record -1, 100.01, NaN or infinity for a 100-mark assignment. | Each invalid mark is rejected; no invalid mark is saved. | To be completed during execution |
| SYS-11 | System | Lecturer records mark 60, then corrects it to 70 with a reason. Inspect audit log. | Current mark is 70; audit log retains old 60, new 70, lecturer and reason. | To be completed during execution |
| SYS-12 | System | HoD requests changes to a mark. Lecturer corrects/resubmits the mark for moderation. | Requested-change decision is recorded; mark cannot be released before approval. | To be completed during execution |
| SYS-13 | System | HoD approves a mark, then releases it. | Mark status changes to Approved and then Released; moderation decision is recorded. | To be completed during execution |
| SYS-14 | System | Lecturer or Student attempts HoD moderation/release; a different lecturer attempts to mark another lecturer's assignment. | Unauthorised actions are rejected. | To be completed during execution |
| SYS-15 | Acceptance | Student checks the assignment list after a whole-class extension. | Revised class deadline is displayed to every enrolled student. | To be completed during execution; current service stores the extension but does not itself provide a student-list UI |
| SYS-16 | Acceptance | Student checks marks before and after HoD release. | Mark is not visible as released before approval/release; released mark and feedback become visible afterward. | To be completed during execution; verify whether the current UI supports this view |

## Important implementation limitations to record

1. `WorkflowService::submit()` records a path, timestamp, version and status; it does not read the sample file or persist its contents. Do not claim file upload/content validation has been tested.
2. The available project is a service/database implementation and does not yet provide a complete interactive role-based UI for every step. SYS-15 and SYS-16 are acceptance requirements that may need UI implementation or a documented manual database/service demonstration.
3. The whole-class extension is stored in `whole_class_extensions`; the student-facing assignment-list display must be verified separately. A successful service test alone does not prove that the deadline is displayed to students.
4. Fill in Actual result / evidence only after running each test. The expected result is not evidence of execution.
