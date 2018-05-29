/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __BLUENRG1DISCOVEREDCHARACTERISTIC_H__
#define __BLUENRG1DISCOVEREDCHARACTERISTIC_H__

#include "ble/DiscoveredCharacteristic.h"

class BlueNRG1GattClient; /* forward declaration */

class BlueNRG1DiscoveredCharacteristic : public DiscoveredCharacteristic {
public:

  void setup(BlueNRG1GattClient         *gattcIn,
             Gap::Handle_t            connectionHandleIn,
             DiscoveredCharacteristic::Properties_t    propsIn,
             GattAttribute::Handle_t  declHandleIn,
             GattAttribute::Handle_t  valueHandleIn,
             GattAttribute::Handle_t  lastHandleIn);

  void setup(BlueNRG1GattClient         *gattcIn,
             Gap::Handle_t            connectionHandleIn,
             UUID   uuidIn,
             DiscoveredCharacteristic::Properties_t    propsIn,
             GattAttribute::Handle_t  declHandleIn,
             GattAttribute::Handle_t  valueHandleIn,
             GattAttribute::Handle_t  lastHandleIn);


  void setLastHandle(GattAttribute::Handle_t  lastHandleIn);
};

#endif /* __BLUENRG1DISCOVEREDCHARACTERISTIC_H__ */
