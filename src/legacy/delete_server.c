#include "include.h"
#include "../server/current_backups.h"

int do_delete_server(const char *basedir, const char *backup, const char *client, struct config *cconf)
{
	int a=0;
	int i=0;
	int ret=0;
	int found=0;
	struct bu *arr=NULL;
	unsigned long index=0;

	logp("in do_delete\n");

	if(get_current_backups(basedir, &arr, &a, 1))
	{
		return -1;
	}

	write_status(client, STATUS_DELETING, NULL, cconf);

	if(backup && *backup) index=strtoul(backup, NULL, 10);

	for(i=0; i<a; i++)
	{
		if(backup && *backup)
		{
			if(!found
			  && (!strcmp(arr[i].timestamp, backup)
				|| arr[i].index==index))
			{
				if(arr[i].deletable)
				{
					found=1;
					async_write_str(CMD_GEN, "ok");
					if(delete_backup(basedir,
						arr, a, i, client))
					{
						free_current_backups(&arr, a);
						ret=-1;
						goto end;
					}
					break;
				}
				else
				{
					async_write_str(CMD_ERROR, "backup not deletable");
					free_current_backups(&arr, a);
					ret=-1;
					goto end;
				}
				break;
			}
		}
	}

	if(backup && *backup && !found)
	{
		async_write_str(CMD_ERROR, "backup not found");
		ret=-1;
	}

end:
	free_current_backups(&arr, a);
	return ret;
}