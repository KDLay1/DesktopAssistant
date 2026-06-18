# Codex Report 2026-06-18

## What changed

- Replaced the timeline placeholder page with a simple working implementation.
- Refreshed pomodoro stats automatically when switching to the stats page.
- Fixed manual bill creation so income is no longer stored as a refund.
- Fixed CSV import so newly created finance categories no longer use invalid primary category IDs.

## Verification

- Rebuilt the app successfully with:
  - `D:\Qt\Tools\CMake_64\bin\cmake.exe --build build\Desktop_Qt_6_11_1_MinGW_64_bit-Debug --parallel 4`

## Remaining follow-ups

- Course-page overlapping-course deletion is still risky and should be narrowed to single-course deletion.
- Finance query error propagation is still weak in some paths.
- The current timeline page is intentionally simple and can be polished later if needed.
