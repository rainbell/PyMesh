#pragma once
#include <string>
#include <tr1/memory>
#include <Environment.h>
#include <Exception.h>
#include <OpenCLWrapper.h>

class OpenCLMultiplyTest : public ::testing::Test, public OpenCLWrapper {
    protected:
        virtual void SetUp() {
            std::string root_dir = Environment::get_required("PYMESH_PATH");
            m_kernel_path = root_dir + "/tests/learning_tests/opencl_kernels/";
            init_platform();
            init_device(false);
            init_context();
            init_queue();
        }

        void load_program(const std::string& kernel_file) {
            init_program_from_file(m_kernel_path + kernel_file);
            build_program();
            init_kernels();
        }

        void set_vector_width(size_t vector_width) {
            m_vector_width = vector_width;
        }

        void test_kernel() {
            compute_work_partition();
            prepare_kernel();
            run_kernel();
            validate_results();
        }

    private:
        void compute_work_partition() {
            size_t data_size = get_data_size();
            m_num_work_items = (data_size / m_vector_width) * m_vector_width;
        }

        void prepare_kernel() {
            FloatArray data = generate_data();
            const size_t padded_size = m_num_work_items * m_vector_width;
            m_input = create_float_buffer(padded_size, data.data());
            m_output = create_zero_float_buffer(padded_size);

            SET_2_KERNEL_ARGS(m_kernel, m_input, m_output);
        }

        void run_kernel() {
            execute_kernel(1, &m_num_work_items, NULL);
        }

        void validate_results() {
            const size_t data_size = get_data_size();
            FloatArray result(data_size);
            read_from_buffer(m_output, 0, sizeof(float) * data_size, result.data());

            for (size_t i=0; i<data_size; i++) {
                ASSERT_FLOAT_EQ(i*2, result[i]);
            }
        }

        FloatArray generate_data() {
            const size_t data_size = get_data_size();
            const size_t padded_size = m_num_work_items * m_vector_width;
            FloatArray data(padded_size);
            for (size_t i=0; i<data_size; i++) {
                data[i] = i;
            }
            return data;
        }

        size_t get_data_size() const {
            return 1024;
        }

    private:
        std::string m_kernel_path;
        size_t m_num_work_items;
        size_t m_vector_width;
        cl_mem m_input;
        cl_mem m_output;
};

TEST_F(OpenCLMultiplyTest, Standard) {
    load_program("multby2.cl");
    set_kernel("multby2");
    set_vector_width(1);
    test_kernel();
}

TEST_F(OpenCLMultiplyTest, WithVector) {
    load_program("multby2_v2.cl");
    set_kernel("multby2_v2");
    set_vector_width(2);
    cl_long local_used = get_kernel_local_mem_size();
    cl_long private_used = get_kernel_private_mem_size();
    std::cout << "local used: " << local_used << std::endl;
    std::cout << "private used: " << private_used << std::endl;
    test_kernel();
}

TEST_F(OpenCLMultiplyTest, RegisterSpill) {
    load_program("multby2_v3.cl");
    set_kernel("multby2_v3");
    cl_long local_used = get_kernel_local_mem_size();
    cl_long private_used = get_kernel_private_mem_size();
    std::cout << "local used: " << local_used << std::endl;
    std::cout << "private used: " << private_used << std::endl;
    set_vector_width(8);
    ASSERT_THROW(test_kernel(), RuntimeError);
}

