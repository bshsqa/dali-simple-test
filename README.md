# DALi Simple Test

Small standalone DALi samples used to validate focused framework changes.

## Accessibility Smoke

`dali-simple-test` creates a small Toolkit UI, sets accessibility role/name/description/state/relations/attributes, and uses a mock AT-SPI client to query and activate the resulting `Accessible` objects without requiring a real screen reader.

Build:

```sh
cd build/tizen
cmake --fresh -DCMAKE_INSTALL_PREFIX="$DESKTOP_PREFIX" .
make -j8 install
```
