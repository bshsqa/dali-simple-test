# DALi Simple Test

Small standalone DALi samples used to validate focused framework changes.

## accessibility-refactoring-test.example

`accessibility-refactoring-test.example` creates a small Toolkit UI, sets accessibility role/name/description/value/state/relation/attribute/scrollable/modal/hidden metadata, and uses a mock accessibility client to query and activate the resulting `Accessible` objects without requiring a real screen reader.

Build:

```sh
cd build/tizen
cmake --fresh -DCMAKE_INSTALL_PREFIX="$DESKTOP_PREFIX" .
make -j8 install
```
