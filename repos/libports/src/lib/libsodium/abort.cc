#include <base/env.h>
#include <base/printf.h>
#include <base/sleep.h>
#include <base/thread.h>

extern "C" void abort(void)
{
	using namespace Genode;

	Thread_base * myself = Thread_base::myself();
	char thread_name[64] = { "unknown" };

	if (myself) myself->name(thread_name, sizeof(thread_name));
	PWRN("abort called - thread: '%s'", thread_name);

	/* Notify the parent of failure */
	if (!strcmp("main", thread_name, sizeof(thread_name)))
		env()->parent()->exit(1);

	sleep_forever();
}

