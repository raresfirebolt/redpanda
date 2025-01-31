// Copyright 2020 Redpanda Data, Inc.
//
// Use of this software is governed by the Business Source License
// included in the file licenses/BSL.md
//
// As of the Change Date specified in that file, in accordance with
// the Business Source License, use of this software will be governed
// by the Apache License, Version 2.0

#include "raft/probe.h"

#include "config/configuration.h"
#include "model/fundamental.h"
#include "prometheus/prometheus_sanitize.h"

#include <seastar/core/metrics.hh>

namespace raft {

std::vector<ss::metrics::label_instance>
probe::create_metric_labels(const model::ntp& ntp) {
    namespace sm = ss::metrics;
    auto ns_label = sm::label("namespace");
    auto topic_label = sm::label("topic");
    auto partition_label = sm::label("partition");
    return {
      ns_label(ntp.ns()),
      topic_label(ntp.tp.topic()),
      partition_label(ntp.tp.partition()),
    };
}

void probe::setup_metrics(const model::ntp& ntp) {
    namespace sm = ss::metrics;
    auto labels = create_metric_labels(ntp);

    _metrics.add_group(
      prometheus_sanitize::metrics_name("raft"),
      {sm::make_counter(
         "received_vote_requests",
         [this] { return _vote_requests; },
         sm::description("Number of vote requests received"),
         labels),
       sm::make_counter(
         "received_append_requests",
         [this] { return _append_requests; },
         sm::description("Number of append requests received"),
         labels),
       sm::make_counter(
         "sent_vote_requests",
         [this] { return _vote_requests_sent; },
         sm::description("Number of vote requests sent"),
         labels),
       sm::make_counter(
         "replicate_ack_all_requests",
         [this] { return _replicate_requests_ack_all; },
         sm::description(
           "Number of replicate requests with quorum ack consistency"),
         labels),
       sm::make_counter(
         "replicate_ack_leader_requests",
         [this] { return _replicate_requests_ack_leader; },
         sm::description(
           "Number of replicate requests with leader ack consistency"),
         labels),
       sm::make_counter(
         "replicate_ack_none_requests",
         [this] { return _replicate_requests_ack_none; },
         sm::description(
           "Number of replicate requests with no ack consistency"),
         labels),
       sm::make_counter(
         "done_replicate_requests",
         [this] { return _replicate_requests_done; },
         sm::description("Number of finished replicate requests"),
         labels),
       sm::make_counter(
         "log_flushes",
         [this] { return _log_flushes; },
         sm::description("Number of log flushes"),
         labels),
       sm::make_counter(
         "log_truncations",
         [this] { return _log_truncations; },
         sm::description("Number of log truncations"),
         labels),
       sm::make_counter(
         "leadership_changes",
         [this] { return _leadership_changes; },
         sm::description("Number of leadership changes"),
         labels),
       sm::make_counter(
         "replicate_request_errors",
         [this] { return _replicate_request_error; },
         sm::description("Number of failed replicate requests"),
         labels),
       sm::make_counter(
         "heartbeat_requests_errors",
         [this] { return _heartbeat_request_error; },
         sm::description("Number of failed heartbeat requests"),
         labels),
       sm::make_counter(
         "recovery_requests_errors",
         [this] { return _recovery_request_error; },
         sm::description("Number of failed recovery requests"),
         labels)});
}

} // namespace raft
