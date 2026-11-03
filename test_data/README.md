# Moderated LMS – Sample Test Data

All people and submissions in this folder are fictional and created only for software testing. The source files are small illustrative programming submissions, not real student work.

## Sample assignment

- Course: SCI101 – Introduction to Computer Programming
- Assignment: Programming Fundamentals Assignment
- Maximum mark: 100
- Base deadline: 10 November 2026, 23:59
- Daily late penalty: 10% of the maximum mark (10 marks per calendar day)
- More than 3 calendar days late: submission is accepted/recorded but final mark is zero

## Files

| File | Intended scenario |
|---|---|
| `submissions/Student_A_OnTime.cpp` | Submitted on the deadline |
| `submissions/Student_B_OneDayLate.cpp` | One calendar day late |
| `submissions/Student_C_ThreeDaysLate.cpp` | Three calendar days late |
| `submissions/Student_D_FourDaysLate.cpp` | More than three days late; zero mark |
| `submissions/Student_E_Resubmission_v1.cpp` | First submission version |
| `submissions/Student_E_Resubmission_v2.cpp` | Second/current submission version |
| `submissions/Student_F_IndividualExtension.cpp` | Submitted within an individual extension |
| `submissions/Student_G_ClassExtension.cpp` | Submitted within a whole-class extension |
| `submissions/Student_Invalid_Empty.cpp` | Empty/invalid submission example |

## Expected calculations

For a maximum mark of 100, penalty = 10 × calendar days late for 1–3 days. Thus a raw mark of 80 becomes 70 after 1 day and 50 after 3 days. At 4 days late, the final mark is 0 regardless of the raw mark.

## How to use these files

The current `WorkflowService::submit()` method accepts a file path string and a supplied submission timestamp. In the automated tests, test paths are passed as strings. For manual demonstration, use the path to the relevant sample `.cpp` file and record the matching submission timestamp. The current service does not read the source file bytes; do not describe this as an end-to-end file upload test.
