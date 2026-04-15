# Editing Rules

## Scope

- Only change what the task requires
- Do not refactor surrounding code unless it directly blocks the task
- Do not add features, flags, or configurability beyond what is asked
- Do not add comments or docstrings to code you did not change

## Tests

- Update tests when changing tested logic
- New behavior must have new tests
- Do not delete tests unless the covered behavior is removed

## Docs

- Update docs when changing public interfaces or user-facing behavior
- Do not update docs for internal refactors

## Dependencies

- Search for known 0-day and supply-chain attacks before adding any new dependency

## Validation

- Validate only at system boundaries (user input, external APIs, file I/O)
- Trust internal code and framework guarantees
