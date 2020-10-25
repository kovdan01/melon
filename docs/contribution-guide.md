# Contribution guide

Nothing is just pushed to the main (`develop`) branch without appropriate procedure. Every change is firstly made in a separate branch in forked repository, and then a pull request to original's `develop` is created. See more details below.

### Fork the repository

This can be done with GitHub interface.

### If already forked, sync your fork's `develop` with original (upstream) repository

An instruction could be found [here](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/syncing-a-fork)

### Create a new branch in your forked repository

```bash
git checkout -b some_descriptive_branch_name
```

### Make changes in this branch

Remember that changes should be atomic when possible: one commit/pull request should solve one problem.

### Give a nice commit message for your changes

Nice commit message must follow these rules:

1. Use imperative form for verbs. Ex: "Fix UB in search algorithm" instead of "Fixed UB in search algorithm".
2. Each commit message consists of header and payload. Payload can be omitted if not needed.
3. Payload text must be aligned as max as 80 characters per string. Header should not have more than 50 characters if it is possible. Anyway, 80 characters is a strict limit.
4. There must be an empty line between header and payload.
5. If you need to include a list in the payload, use markdown-like syntax.
6. Code parts included in commit message must be enclosed in ``.

### Push the changes to the forked repository

### Create a pull request from your branch to upstream's `develop`

### Wait until your changes are approved
