#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include <switch/types.h>
#include <atomic>
#include <queue>

#include "vector2.hpp"
#include "logwriter.hpp"
#include "transformation.hpp"
#include "inputevent.hpp"
#include "rendersurface.hpp"

//this struct is passed to the input thread as a paramater
struct InputThreadParameter {
    std::atomic<bool>* thread_request_to_exit;
    std::queue<InputEvent>* thread_input_queue;
    Mutex* thread_input_mutex;
};

class Window : public LogWriter, public RenderSurface {
    private:
        static Window* instance;
        static bool is_initialized;
        bool is_valid;

        u32* frame_buffer;

        std::atomic<bool> request_to_exit;          //set to false by default. when set to true notifies the input thread that it should stop executing
        bool thread_initialized;                    //whether or not the external thread has been correctly initialized
        bool thread_started;                        //whether or not the external thread has been correctly started
        InputThreadParameter thread_parameter;      //we will pass this struct to the input thread (it is not elegant, don't even tell me)
        std::queue<InputEvent> input_queue;         //here we will store all the event that we registered, ready to be accessed by the user
        Thread input_thread;                        //in this thread we process all the inputs
        Mutex input_mutex;                          //a mutex to protect shared resources (basically the event queue)

        Window();
        virtual ~Window() {}

        static std::vector<Button> controllerBitMask(const u64 controllerState); //this does a bitmask on u32 to see which button has been pressed/released
        static void inputManagement(void* theParameter); //this will be launched in a separate thread

    public:
        Window(const Window& theWindow) = delete;
        Window& operator=(const Window& theWindow) = delete;

        bool isValid() const { return is_valid; }
        void update();
        void clear(); //clears the framebuffer
        void setPixelCurrent(const int thePosition, const Color& theColor); //fills the specified pixel window with the specified color/alpha (takes theTransformation into account)
        bool getInputEvents(InputEvent& theBuffer);
        bool isOpen() const { return appletMainLoop(); }
        static Window* getInstance();
        static void destroy();
};

#endif
