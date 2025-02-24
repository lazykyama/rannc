//
// Created by Masahiro Tanaka on 2020/05/20.
//

#include "DeploymentSerializer.h"

namespace rannc {

    struct DeploymentState {
        Deployment deployment;
        int world_size;
        long dev_mem;

        MSGPACK_DEFINE(deployment, world_size, dev_mem);
    };

    void save(const std::string& file, const Deployment& deployment,
              int world_size, long dev_mem) {

        DeploymentState state{deployment, world_size, dev_mem};

        const auto dep_data = serialize(state);

        std::ofstream out(file, std::ios::out | std::ios::binary);
        if (!out) {
            throw std::invalid_argument("Failed to open file: " + file);
        }
        out.write(reinterpret_cast<const char*>(&dep_data[0]), dep_data.size());
        out.close();
    }

    Deployment load(const std::string& file, int world_size, long dev_mem) {

        std::ifstream input(file, std::ios::in | std::ios::binary);
        if (!input) {
            throw std::invalid_argument("Failed to open file: " + file);
        }

        std::vector<char> buffer(std::istreambuf_iterator<char>(input), {});

        DeploymentState state = deserialize<DeploymentState>(buffer);

        if (state.world_size != world_size || state.dev_mem != dev_mem) {
            std::stringstream ss;
            ss << "Deployment state does not match: world_size=" << state.world_size
                << " dev_mem=" << state.dev_mem;
            throw std::invalid_argument(ss.str());
        }

        return state.deployment;
    }
}