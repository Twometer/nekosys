#ifndef _NK_FUNCTION_H
#define _NK_FUNCTION_H

/* taken from https://www.codeproject.com/Articles/313312/Cplusplus11-Lambda-Storage-Without-libcplusplus */
/*   modifications done: fixed missing this-> so that it would compile */

namespace nk
{

    template <typename T>
    class FunctionExecutor
    {
    };

    template <typename Out, typename... In>
    class FunctionExecutor<Out(In...)>
    {
    public:
        Out operator()(In... in)
        {
            // assert(lambda != nullptr);
            return executeFunction(lambda, in...);
        }

    protected:
        FunctionExecutor(void *&lambda) : lambda(lambda) {}

        ~FunctionExecutor() {}

        template <typename T>
        void generateExecutor(T const &lambda)
        {
            executeFunction = [](void *lambda, In... arguments) -> Out {
                return ((T *)lambda)->operator()(arguments...);
            };
        }

        void receiveExecutor(FunctionExecutor<Out(In...)> const &other)
        {
            executeFunction = other.executeFunction;
        }

    private:
        void *&lambda;
        Out (*executeFunction)(void *, In...);
    };

    template <typename... In>
    class FunctionExecutor<void(In...)>
    {
    public:
        void operator()(In... in)
        {
            //  assert(lambda != nullptr);
            executeFunction(lambda, in...);
        }

    protected:
        FunctionExecutor(void *&lambda) : lambda(lambda) {}

        ~FunctionExecutor() {}

        template <typename T>
        void generateExecutor(T const &lambda)
        {
            executeFunction = [](void *lambda, In... arguments) {
                return ((T *)lambda)->operator()(arguments...);
            };
        }

        void receiveExecutor(FunctionExecutor<void(In...)> const &other)
        {
            executeFunction = other.executeFunction;
        }

    private:
        void *&lambda;
        void (*executeFunction)(void *, In...);
    };

    template <typename T>
    class Function
    {
    };

    template <typename Out, typename... In>
    class Function<Out(In...)> : public FunctionExecutor<Out(In...)>
    {
    public:
        Function() : FunctionExecutor<Out(In...)>(lambda),
                     lambda(nullptr), deleteFunction(nullptr), copyFunction(nullptr)
        {
        }

        Function(Function<Out(In...)> const &other) : FunctionExecutor<Out(In...)>(lambda),
                                                      lambda(other.copyFunction ? other.copyFunction(other.lambda) : nullptr),
                                                      deleteFunction(other.deleteFunction), copyFunction(other.copyFunction)
        {
            this->receiveExecutor(other);
        }

        template <typename T>
        Function(T const &lambda) : FunctionExecutor<Out(In...)>(this->lambda), lambda(nullptr)
        {
            // Copy should set all variables
            copy(lambda);
        }

        ~Function()
        {
            if (deleteFunction != nullptr)
                deleteFunction(lambda);
        }

        Function<Out(In...)> &operator=(Function<Out(In...)> const &other)
        {
            this->lambda = other.copyFunction ? other.copyFunction(other.lambda) : nullptr;
            receiveExecutor(other);
            this->deleteFunction = other.deleteFunction;
            this->copyFunction = other.copyFunction;
            return *this;
        }

        template <typename T>
        Function<Out(In...)> &operator=(T const &lambda)
        {
            copy(lambda);
            return *this;
        }

        operator bool()
        {
            return lambda != nullptr;
        }

    private:
        template <typename T>
        void copy(T const &lambda)
        {
            if (this->lambda != nullptr)
                deleteFunction(this->lambda);
            this->lambda = new T(lambda);

            this->generateExecutor(lambda);

            deleteFunction = [](void *lambda) {
                delete (T *)lambda;
            };

            copyFunction = [](void *lambda) -> void * {
                return lambda ? new T(*(T *)lambda) : nullptr;
            };
        }

        void *lambda;
        void (*deleteFunction)(void *);
        void *(*copyFunction)(void *);
    };

}; // namespace nk

#endif