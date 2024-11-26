//    OpenVPN -- An application to securely tunnel IP networks
//               over a single port, with support for SSL/TLS-based
//               session authentication and key exchange,
//               packet encryption, packet authentication, and
//               packet compression.
//
//    Copyright (C) 2012- OpenVPN Inc.
//
//    SPDX-License-Identifier: MPL-2.0 OR AGPL-3.0-only WITH openvpn3-openssl-exception
//

#ifndef OPENVPN_APPLECRYPTO_CF_CFHOST_H
#define OPENVPN_APPLECRYPTO_CF_CFHOST_H

#include <openvpn/apple/cf/cf.hpp>

namespace openvpn::CF {
OPENVPN_CF_WRAP(Host, host_cast, CFHostRef, CFHostGetTypeID)
}

#endif
