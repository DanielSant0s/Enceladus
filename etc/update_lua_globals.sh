if [ ! -f ".vscode/settings.json" ]; then
    echo settings.json is missing, creating blank one...
    echo "{">.vscode/settings.json
    echo "}">>.vscode/settings.json
fi
CNT=0
GLOBALS=$(find "src/" -type f -a \( -iname \*.* \) | xargs grep -Eo "lua_setglobal.*;" | grep -Eo "\".*\"" | tr -d '"')
for item in ${GLOBALS}; do
    jq --arg order "$item" '.["Lua.diagnostics.globals"] |= (. + [$order] | unique)' .vscode/settings.json >.vscode/settings.json.bak
    cat .vscode/settings.json.bak > .vscode/settings.json
    CNT=$((CNT+1))
done
echo found $CNT lua constants.
rm -f .vscode/settings.json.bak