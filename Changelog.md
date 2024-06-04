# Changelog

## Verison 2.1

Previous implementation did not fully account for total size of a call to
calloc. This version should fix this.

## Version 2.0

Minor refactorings.

Change internal implementation of linked list to be doubly-linked.

Add support for calloc.

Add pthreads to mutex-lock malloc/calloc/free.

## Version 1.0

Initial version with working `AnotherMemCheck`.
