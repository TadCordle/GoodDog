@echo off
if exist web\ del web\
if not exist web\ mkdir web\
emcc GoodDog.cpp WobblyRender.cpp Game.cpp -s WASM=1 -o web/gooddog.html -D_DEFAULT_SOURCE -Os -I. -I C:/raylib/src -I C:/raylib/src/external -L. -L C:/raylib/src -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --preload-file resources/ --shell-file C:/raylib/src/shell.html C:/raylib/src/web/libraylib.a -DPLATFORM_WEB -s EXPORTED_FUNCTIONS=["_free","_malloc","_main"] -s EXPORTED_RUNTIME_METHODS=ccall
xcopy resources/ web/resources/ /E
ren web\gooddog.html index.html