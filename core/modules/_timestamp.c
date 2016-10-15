#include <rte_config.h>
#include <rte_ether.h>
#include <rte_ip.h>
#include <rte_tcp.h>

#include "../module.h"
#include "../utils/histogram.h"

static inline void timestamp_packet(struct snbuf *pkt, uint64_t time) {
  uint8_t *avail = static_cast<uint8_t *>(snb_head_data(pkt)) +
                   sizeof(struct ether_hdr) + sizeof(struct ipv4_hdr) +
                   sizeof(struct tcp_hdr);
  *avail = 1;
  uint64_t *ts = (uint64_t *)(avail + 1);
  *ts = time;
}

class Timestamp : public Module {
 public:
  virtual void ProcessBatch(struct pkt_batch *batch);

  static const gate_idx_t kNumIGates = 1;
  static const gate_idx_t kNumOGates = 1;

  static const std::vector<struct Command> cmds;
};

const std::vector<struct Command> Timestamp::cmds = {};

void Timestamp::ProcessBatch(struct pkt_batch *batch) {
  uint64_t time = get_time();

  for (int i = 0; i < batch->cnt; i++) timestamp_packet(batch->pkts[i], time);

  run_next_module(this, batch);
}

ModuleClassRegister<Timestamp> timestamp(
    "Timestamp", "timestamp",
    "marks current time to packets (paired with Measure module)");