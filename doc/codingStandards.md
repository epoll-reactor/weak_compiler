##### Contents

[Naming](#Naming)

[Formatting](#Formatting)

[Error handling](#Error-handling)

## Naming
| Subject       | Style         |
| ------------- | ------------- |
| Class         | PascalCase    |
| Function      | PascalCase    |
| Variable      | PascalCase    |
| Namespace     | camelCase     |
| Source file   | camelCase     |
| Directory     | camelCase     |
| Document      | camelCase     |

## Formatting
Clang-format with built-in LLVM style.

## Error handling
Avoid exceptions at all. Instead of this, better to return abstract Maybe type or error code from STL instead.
