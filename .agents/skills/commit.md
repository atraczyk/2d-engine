# Commit

## Grouping

Check `git diff` and `git diff --staged` before writing a message. One commit per logical unit of work — do not mix unrelated changes. Unstage unrelated files and commit them separately.

## Message format

`type(scope): subject` — conventional commits
Types: `feat`, `fix`, `chore`, `docs`, `refactor`, `test`

- Subject ≤50 chars
- Body lines ≤72 chars, blank line between subject and body

```
feat(auth): add JWT refresh token support

Tokens now auto-refresh 5 minutes before expiry. Refresh
logic is in src/auth/refresh.ts.
```

```
fix(api): correct pagination offset calculation
```
