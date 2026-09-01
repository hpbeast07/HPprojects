# Contributing to ServerRules

Thank you for your interest in contributing!

## Getting Started

1. Fork the repository.
2. Clone your fork: `git clone https://github.com/your-username/ServerRules.git`
3. Create a feature branch: `git checkout -b feature/my-feature`
4. Make your changes, following the code style guidelines below.
5. Run tests: `./gradlew :serverrules-core:test`
6. Build: `./gradlew build`
7. Commit your changes with a clear message.
8. Open a Pull Request against the `main` branch.

## Code Style

- **Java 21** — use records, sealed classes, switch expressions where appropriate
- Follow **SOLID** principles
- Write **JavaDoc** for all public API methods
- Add **unit tests** for new core logic in `serverrules-core`
- Keep platform-specific code in `serverrules-paper` only

## Pull Request Guidelines

- Keep PRs focused on a single change
- Update `CHANGELOG.md` under an `[Unreleased]` section
- Update `README.md` if you add new commands or config options
- Ensure all existing tests pass

## Reporting Issues

Use GitHub Issues. Please include:
- Server software and version (Paper 1.21.1, etc.)
- ServerRules version
- Relevant console output
- Steps to reproduce

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
