# Git Workflow

## Rules

- Never force-push to `main` or `master`
- Never skip commit hooks (`--no-verify`)
- Never amend a commit that has already been pushed
- Never reset or restore files without explicit instruction
- Do not create branches, tags, or releases unless asked
- Do not push to remote unless asked
- Do not merge or rebase unless asked

## Branching

Trunk-based development. Work directly on `main` for small changes;
use short-lived feature branches for larger work.

Default branch: `main`
