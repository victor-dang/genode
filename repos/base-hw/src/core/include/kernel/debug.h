#include <kernel/thread.h>
#include <kernel/irq.h>
#include <serial.h>
#include <platform_console.h>

namespace Kernel { struct Serial_irq; }

struct Kernel::Serial_irq : Kernel::Irq
{
	Serial_irq() : Kernel::Irq(Genode::Serial::IRQ, *cpu_pool()->executing_cpu()) {
		enable(); }

	void occurred()
	{
		if (!Genode::platform_console()->get_char()) return;

		PINF("Dump all thread states");

		for (Thread * thread = thread_list().first(); thread;
		     thread = thread->Genode::List<Thread>::Element::next()) {
			thread->print_activity();
			if (thread->pd() == core_pd()) thread->print_backtrace();
		}

		PINF("Dump finished");
	}

	static Serial_irq& singleton();
};

Kernel::Serial_irq& Kernel::Serial_irq::singleton()
{
	static Serial_irq si;
	return si;
}

