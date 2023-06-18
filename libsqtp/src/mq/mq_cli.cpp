#include "mq/mq_cli.h"
#include "mq/mq_cli_kafka.h"
#include <assert.h>
#if SUPPORT_MQ
namespace sq
{
	mq_cli*mq_cli_factory::create_mq_cli(const char*class_name)
	{
		mq_cli*cli = nullptr;
		if (strcmp(class_name, "mq_cli_kafka") == 0)
			cli = new mq_cli_kafka();
		else
			assert(false);

		return cli;
	}
}
#endif
