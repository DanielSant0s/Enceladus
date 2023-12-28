UPDATING=true
if [ ! -f ".vscode/settings.json" ]; then
    mkdir -p .vscode
    echo settings.json is missing, creating from scratch
else
    UPDATING=false
    echo updating existing settings.json
    mv .vscode/settings.json .vscode/settings.json.bak
fi

GLOBALS=$(find "src/" -type f -a \( -iname \*.* \) | xargs grep -Eo "lua_setglobal.*;" | grep -Eo "\".*\"" | tr -d '"')
GLOBALS+=$'\n'
GLOBALS+=$(find "src/" -type f -a \( -iname \*.* \) | xargs grep -Eo "lua_register.*;" | grep -Eo "\".*\"" | tr -d '"')
echo $GLOBALS
jq -n --arg inarr "${GLOBALS}" '{ "Lua.diagnostics.globals": $inarr | split("\n") }' > .vscode/settings.json.new
if [ "$UPDATING" == false ]; then
jq -s '.[0] * .[1]' .vscode/settings.json.bak .vscode/settings.json.new > .vscode/settings.json
else
mv .vscode/settings.json.new .vscode/settings.json
fi
echo found $(echo "$GLOBALS" | wc -l) lua constants.
rm -f .vscode/settings.json.bak
rm -f .vscode/settings.json.new