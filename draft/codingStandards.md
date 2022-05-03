##### Contents

[Naming](#Naming)

[Formatting](#Formatting)

[Error handling](#Error-handling)

## Naming
| Subject            | Style         |
| ------------------ | ------------- |
| Class              | PascalCase    |
| Function           | PascalCase    |
| Variable           | PascalCase    |
| Namespace          | camelCase     |
| Source file        | PascalCase    |
| External Directory | camelCase     |
| Document           | camelCase     |

## Formatting

### Code style
Code should be formatted with Clang-format (LLVM style).

### Header guard
Header guard should be represented in format (PROJECT_NAME) _ (DIRS_PATH) _ (FILE_NAME) _ HPP.

## Error handling
Completely avoid exceptions. Instead of this, in context of compiler, it makes no sense to create exceptions when
error occurred. Better to print warning message or terminate compilation process on error since in this case
there is nothing more to do. All exceptions thrown from STL functions should be handled internally.

## Testing
All described as interface code (e.g header files declarations) MUST be tested, but internal code not.
