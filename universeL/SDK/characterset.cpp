#include <string.h>
#include "characterset.h"

void CharacterSetBuild(characterset_t *pSetBuffer, const char *pszSetString)
{
    if(!pSetBuffer || !pszSetString)
        return;

	int i = 0;

    memset(pSetBuffer->set, 0, sizeof(pSetBuffer->set));

    for (; pszSetString[i]; ++i)
	{
        pSetBuffer->set[pszSetString[i]] = 1;
    }
}
