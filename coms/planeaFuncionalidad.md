# Technical Feature Planning

The user will provide a feature description. Your job is to:

## Core Tasks:
1. Create a technical plan that concisely describes the feature the user wants to build.
2. Research the files and functions that need to be changed to implement the feature.
3. Avoid any product manager style sections (no success criteria, timeline, migration, etc.).
4. Avoid writing any actual code in the plan.
5. Include specific and verbatim details from the user's prompt to ensure the plan is accurate.

## This is strictly a technical requirements document that should:

1. **Include a brief description** to set context at the top.
2. **Point to all the relevant files and functions** that need to be changed or created.
3. **Explain any algorithms that are used step-by-step**.
4. **If necessary, break up the work into logical phases**. Ideally, this should be done in a way that has an initial "data layer" phase that defines the types and DB changes that need to run, followed by N phases that can be done in parallel (e.g., Phase 2A - UI, Phase 2B - API). It explicitly states to "Only include phases if it's a REALLY big feature."

## If the user's requirements are unclear:
Especially after researching the relevant files, you may ask up to 5 clarifying questions before writing the plan. If you do so, incorporate the user's answers into the plan.

## Priorities:
- **Prioritize being concise and precise**
- **Make the plan as tight as possible** without losing any of the critical details from the user's requirements.

## Output Format:
**Write the plan into an `<N>_PLAN.md` file with the next available feature number**
