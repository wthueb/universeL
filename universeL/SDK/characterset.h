#pragma once

struct characterset_t
{
    char set[256];
};

extern void CharacterSetBuild(characterset_t *pSetBuffer, const char *pSetString);

#define IN_CHARACTERSET(SetBuffer, character) ((SetBuffer).set[(character)])
