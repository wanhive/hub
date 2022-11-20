# Introduction

Follow these guidelines while making changes in the project's repository.

# How to contribute

Submit all issues, suggestions, and updates via the repository. Clearly explain a problem and the proposed solution.

You agree to license your "original work" under a compatible permissive license. Find the list of recommended permissive licenses below:

- Apache License 2.0
- BSD 2-clause license
- BSD 3-clause license
- MIT license

## Code of conduct

[Code of conduct](CODE_OF_CONDUCT.md)

# General requirements

* Language standards: C++17
* Format the code in the K&R style.
* The code should be robust, sufficient, and easy to test for correctness.
* The code should be efficient.
* The code should compile fast.
* The code should be easy to read and maintain.
* Prioritize readability and clarity over the running time.
* Ensure thread safety at the class level. Avoid shared states.

# Naming convention

Names should be short and meaningful.

Only ASCII alphanumerics (a-zA-Z0-9), hyphen (-), and underscore (_) are allowed in the names. Names must not include leading numeric digits (0-9) or hyphens. Trailing hyphens are not allowed.

## Source filename

Header and source files containing a class declaration and implementation should be named the same as the class.

In all the other cases, unless otherwise required by any relevant standards, file names should be nouns in lowercase of the ASCII alphabet (a-z). Avoid other allowed characters.

## Class and interface name

Start with a **C**apitalized letter, and capitalize each internal word (PascalCase). Class and Interface names should be nouns. Avoid acronyms and abbreviations.

## Method or function name

Start with a lowercase letter (a-z), and capitalize each internal word (camelCase). Method names should be verbs (except constructors, destructors, and operators).

## Field or variable name

Field or variable names should be mnemonics to provide a good indicator of the intent of their use.

A single-character name is allowed for temporary variables containing basic data types. Common names for temporary variables are i, j, k for integral values, d,e,f for real values,  m,n,p for characters, and w,x,y,z for strings.

Only private fields/variables may start or end with an underscore (_).

Public static variables should be named using upper case letters, a hyphen, an underscore, and digits, always starting with an upper case letter (A-Z).

# Comments

Write your comments in simple English.

Restrict all comments to 100 (ideally 80) characters per line, including white space. Restrict multiline comments to 250 characters. Maintain this limit for optimal readability.

Multiline comments should follow a semantics of general-to-specific details.

Do not spam.

## Code unit comments

Avoid inline comments.

Explain the expected behavior of your code using short inline comments.
