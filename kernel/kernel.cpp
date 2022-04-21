#include "kernel/kernel.hpp"
#include "kernel/tty.hpp"

#include "libcpp/stdio.hpp"

namespace kernel {

void Kernel::Init() {
    tty.Init();

    libcpp::println("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Massa sed elementum tempus egestas. Eget arcu dictum varius duis at consectetur lorem donec massa. Rhoncus dolor purus non enim praesent elementum facilisis leo vel. Viverra orci sagittis eu volutpat odio. Velit egestas dui id ornare arcu odio ut. Porta non pulvinar neque laoreet. Lorem ipsum dolor sit amet consectetur adipiscing elit pellentesque. Sit amet porttitor eget dolor morbi non arcu risus. Porttitor eget dolor morbi non arcu risus quis varius. Vitae ultricies leo integer malesuada nunc. Egestas pretium aenean pharetra magna ac placerat vestibulum lectus. Sed viverra tellus in hac habitasse platea dictumst vestibulum rhoncus. Rhoncus dolor purus non enim praesent elementum facilisis leo vel. Fringilla phasellus faucibus scelerisque eleifend donec pretium vulputate sapien nec. Nunc id cursus metus aliquam.\n");
    libcpp::println("Quam adipiscing vitae proin sagittis nisl. Nulla malesuada pellentesque elit eget gravida cum sociis natoque. Donec et odio pellentesque diam. In cursus turpis massa tincidunt dui ut. Sed cras ornare arcu dui vivamus arcu felis bibendum ut. Purus non enim praesent elementum facilisis. Diam in arcu cursus euismod quis. Non quam lacus suspendisse faucibus interdum posuere lorem ipsum. Vitae proin sagittis nisl rhoncus mattis rhoncus urna. Vel risus commodo viverra maecenas.\n");
    libcpp::println("A scelerisque purus semper eget duis at tellus at urna. Mi tempus imperdiet nulla malesuada pellentesque elit. Pharetra pharetra massa massa ultricies mi quis hendrerit. Amet justo donec enim diam vulputate ut pharetra sit. Viverra mauris in aliquam sem fringilla ut morbi. Mus mauris vitae ultricies leo integer malesuada. Id volutpat lacus laoreet non. Senectus et netus et malesuada fames. Ac tincidunt vitae semper quis lectus nulla at volutpat diam. Sit amet commodo nulla facilisi nullam vehicula ipsum a arcu. Nunc consequat interdum varius sit amet mattis. Nisl condimentum id venenatis a. Id leo in vitae turpis massa.\n");
    libcpp::println("At imperdiet dui accumsan sit amet nulla. Facilisis volutpat est velit egestas dui id ornare arcu. Vitae nunc sed velit dignissim sodales. In hendrerit gravida rutrum quisque non tellus orci. Habitant morbi tristique senectus et netus et malesuada. Faucibus turpis in eu mi bibendum neque egestas. Morbi enim nunc faucibus a pellentesque sit amet. Cras ornare arcu dui vivamus arcu. Adipiscing elit duis tristique sollicitudin nibh sit amet commodo nulla. Amet consectetur adipiscing elit duis tristique sollicitudin nibh sit amet.\n");

    libcpp::printf("The answer to %s is %d.", "the ultimate question of life, the universe and everything", 42);
}

static Kernel instance_{};

Kernel& instance() {
    return instance_;
}

}

extern "C" void kernel_main()
{
    kernel::instance().Init();
}
