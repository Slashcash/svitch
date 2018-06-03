#include "window.hpp"

#include <switch.h>
#include <unistd.h>

#include "drawable.hpp"

Window* Window::instance = nullptr;
bool Window::is_initialized = false;

Window::Window() {
    is_valid = true;
    //GRAPHIC ENVIROMENT
    writeToLog("Initializing graphic enviroment", 1);
    gfxInitDefault();

    //getting a framebuffer pointer & size
    frame_buffer = (u32*)gfxGetFramebuffer(&window_size.x, &window_size.y);
    std::ostringstream graphic_stream;
    graphic_stream << "Using the framebuffer @" << std::hex << frame_buffer << " with a size of " << std::dec << window_size.x << "x" << window_size.y;
    writeToLog(graphic_stream.str());

    //INPUT INITIALIZATION
    //we don't want the newly created thread to exit right away, right?
    writeToLog("Initializing input detection");
    request_to_exit = false;
    thread_initialized = false;
    thread_started = false;

    //initializing the mutex
    mutexInit(&input_mutex);

    //some useful constants for the thread initialization
    size_t thread_stack_size = 1024; //the size for the stack of the separate thread
    int thread_priority = 0x2C; //the priority for this thread, 0x2C is default in nintendo switch
    int thread_cpu = 1; //a value between 0 and 3, we run the thread on the cpu nr 1 IDEA: implement a mechanism in which the cpu to run on is chosen dinamically to improve performances

    //creating a new thread where to manage the inputs
    writeToLog("Creating the input detection thread");
    thread_parameter.thread_request_to_exit = &request_to_exit;
    thread_parameter.thread_input_queue = &input_queue; //creating the struct that we will pass as a parameter
    thread_parameter.thread_input_mutex = &input_mutex;
    Result res = threadCreate(&input_thread, inputManagement, (void*)(&thread_parameter), thread_stack_size, thread_priority, thread_cpu);

    if( R_FAILED(res) ) {
        OPResult op_res(ERR_CREATE_THREAD, R_DESCRIPTION(res));
        writeToLog(op_res);
        is_valid = false;
        return;
    }
    //at this point the thread is initialized and will need to be destroyed
    thread_initialized = true;

    //initializing the mutex
    mutexInit(&input_mutex);

    writeToLog("Starting the input management thread");
    //starting the thread that will manage the input handling
    res = threadStart(&input_thread);
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_START_THREAD, R_DESCRIPTION(res));
        is_valid = false;
        writeToLog(op_res);
        return;
    }

    //at this point the thread is started and will need to be stopped
    thread_started = true;

    std::ostringstream thread_stream;
    thread_stream << "Thread started on core nr " << thread_cpu << " with a priority of " << thread_priority << " and with a stack of " << thread_stack_size << " bytes";
    writeToLog(thread_stream.str());
}

void Window::update() {
    gfxSwapBuffers();
    gfxWaitForVsync();
}

void Window::clear(const Color& theColor) {
    gfxFlushBuffers();

    for( unsigned int y = 0; y < window_size.y; y++ )
        for( unsigned int x = 0; x < window_size.x; x++ )
            setWindowPixel(Coordinate(x, y), theColor);
}

void Window::setWindowPixel(const Coordinate& theCoordinate, const Color& theColor, const Transformation& theTransformation) {
    int pos = mapCoordinatesToLinear(theTransformation.transformCoordinate(theCoordinate));

    if( pos != -1 ) { //if we are inside our window
        frame_buffer[pos] = theColor;
    }
}

int Window::mapCoordinatesToLinear(const Coordinate& theCoordinate) const {
    if( theCoordinate.x < 0 || theCoordinate.y < 0 ) return -1; //if it is outside the range of our window in negative
    if( (unsigned int)theCoordinate.x >= window_size.x || (unsigned int)theCoordinate.y >= window_size.y ) return -1; //or in positive (safe cast now because we know coordinate are both positive)

    else return theCoordinate.y * window_size.x + theCoordinate.x;
}

void Window::draw(const Drawable& theDrawable, const Transformation& theTransformation) {
    theDrawable.draw(*this, theTransformation);
}

Window* Window::getInstance() {
    if( !is_initialized ) {
        instance = new Window();
        is_initialized = true;
    }

    return instance;
}

void Window::inputManagement(void* theParameter) {
    //THIS CONSTANT IS REALLY IMPORTANT, it sets the number of millisecond this thread should sleep after every iteration. Lowering it will make this process heavy but the input detection more sensible
    const useconds_t SLEEP_TIME = 1;

    //getting back all the needed parameter to ease the process
    std::atomic<bool>* trd_request_to_exit = ((InputThreadParameter*)(theParameter))->thread_request_to_exit;
    std::queue<InputEvent>* trd_input_queue = ((InputThreadParameter*)(theParameter))->thread_input_queue;
    Mutex* trd_input_mutex = ((InputThreadParameter*)(theParameter))->thread_input_mutex;

    while( !trd_request_to_exit->load() ) { //waiting for an external request to exit
        //general hid scan
        hidScanInput();

        //checking for the pressed buttons for P1
        u64 key_down_p1 = hidKeysDown(CONTROLLER_P1_AUTO);
        std::vector<Button> pressed = controllerBitMask(key_down_p1);

        for(auto it = pressed.begin(); it < pressed.end(); it++) {
            //CRITICAL
            mutexLock(trd_input_mutex);
            trd_input_queue->push(InputEvent(InputEvent::Type::BUTTON_PRESSED, *it, Player::PLAYER1));
            mutexUnlock(trd_input_mutex);
            //END OF CRITICAL
        }

        //checking for the released buttons for P1
        u64 key_up_p1 = hidKeysUp(CONTROLLER_P1_AUTO);
        std::vector<Button> released = controllerBitMask(key_up_p1);

        for(auto it = released.begin(); it < released.end(); it++) {
            //CRITICAL
            mutexLock(trd_input_mutex);
            trd_input_queue->push(InputEvent(InputEvent::Type::BUTTON_RELEASED, *it, Player::PLAYER1));
            mutexUnlock(trd_input_mutex);
            //END OF CRITICAL
        }

        //checking if there is an external request from the switch to close the app
        /*if( !appletMainLoop() ) {
            //CRITICAL
            mutexLock(trd_input_mutex);
            trd_input_queue->push(InputEvent(InputEvent::Type::WINDOW_CLOSE));
            mutexUnlock(trd_input_mutex);
            //END OF CRITICAL
        }*/

        usleep(SLEEP_TIME*1000);
    }
}

std::vector<Button> Window::controllerBitMask(const u64 controllerState) {
    std::vector<Button> buffer;

    //scanning for all the keys
    if( controllerState & Button::KEY_A ) buffer.push_back(Button::KEY_A);
    if( controllerState & Button::KEY_B ) buffer.push_back(Button::KEY_B);
    if( controllerState & Button::KEY_X ) buffer.push_back(Button::KEY_X);
    if( controllerState & Button::KEY_Y ) buffer.push_back(Button::KEY_Y);
    if( controllerState & Button::KEY_LSTICK ) buffer.push_back(Button::KEY_LSTICK);
    if( controllerState & Button::KEY_RSTICK ) buffer.push_back(Button::KEY_RSTICK);
    if( controllerState & Button::KEY_L ) buffer.push_back(Button::KEY_L);
    if( controllerState & Button::KEY_R ) buffer.push_back(Button::KEY_R);
    if( controllerState & Button::KEY_ZL ) buffer.push_back(Button::KEY_ZL);
    if( controllerState & Button::KEY_ZR ) buffer.push_back(Button::KEY_ZR);
    if( controllerState & Button::KEY_PLUS ) buffer.push_back(Button::KEY_PLUS);
    if( controllerState & Button::KEY_MINUS ) buffer.push_back(Button::KEY_MINUS);
    if( controllerState & Button::KEY_DLEFT ) buffer.push_back(Button::KEY_DLEFT);
    if( controllerState & Button::KEY_DRIGHT ) buffer.push_back(Button::KEY_DRIGHT);
    if( controllerState & Button::KEY_DDOWN ) buffer.push_back(Button::KEY_DDOWN);
    if( controllerState & Button::KEY_DUP ) buffer.push_back(Button::KEY_DUP);
    if( controllerState & Button::KEY_LSTICK_LEFT ) buffer.push_back(Button::KEY_LSTICK_LEFT);
    if( controllerState & Button::KEY_LSTICK_RIGHT ) buffer.push_back(Button::KEY_LSTICK_RIGHT);
    if( controllerState & Button::KEY_LSTICK_UP ) buffer.push_back(Button::KEY_LSTICK_UP);
    if( controllerState & Button::KEY_LSTICK_DOWN ) buffer.push_back(Button::KEY_LSTICK_DOWN);
    if( controllerState & Button::KEY_RSTICK_LEFT ) buffer.push_back(Button::KEY_RSTICK_LEFT);
    if( controllerState & Button::KEY_RSTICK_RIGHT ) buffer.push_back(Button::KEY_RSTICK_RIGHT);
    if( controllerState & Button::KEY_RSTICK_UP ) buffer.push_back(Button::KEY_RSTICK_UP);
    if( controllerState & Button::KEY_RSTICK_DOWN ) buffer.push_back(Button::KEY_RSTICK_DOWN);
    if( controllerState & Button::KEY_SL ) buffer.push_back(Button::KEY_SL);
    if( controllerState & Button::KEY_SR ) buffer.push_back(Button::KEY_SR);

    return buffer;
}

bool Window::getInputEvents(InputEvent& theBuffer) {
    //CRITICAL (accessing the event queue)
    mutexLock(&input_mutex);
    if( input_queue.empty() ) { mutexUnlock(&input_mutex); return false; } //if it is empty we return false
    else {
        theBuffer = input_queue.front();
        input_queue.pop();
        mutexUnlock(&input_mutex);
        //END OF CRITICAL
        return true;
    }
}

void Window::destroy() {
    //exiting gfx
    gfxExit();

    //waiting for the input thread to terminate
    if( Window::getInstance()->thread_started) { //why stopping a thread that didn't even start?
        Window::getInstance()->request_to_exit = true; //asking the thread to terminate
        writeToLog("Waiting the input thread to terminate");
        threadWaitForExit(&Window::getInstance()->input_thread); //waiting for the thread to stop (not checking the result because what would we do with it?)
    }

    if( Window::getInstance()->thread_initialized ) { //why deinitialize if it did not initialize?
        threadClose(&Window::getInstance()->input_thread); //freeing resources
    }

    delete instance;
    instance = nullptr;
    is_initialized = false;
}
