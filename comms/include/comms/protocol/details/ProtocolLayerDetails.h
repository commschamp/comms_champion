//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

namespace comms
{

namespace protocol
{

namespace details
{

template <class T, class R = void>
struct ProtocolLayerEnableIfHasAllMessages { using Type = R; };

template <class T, class Enable = void>
struct ProtocolLayerAllMessagesHelper
{
    using Type = void;
};

template <class T>
struct ProtocolLayerAllMessagesHelper<T, typename ProtocolLayerEnableIfHasAllMessages<typename T::AllMessages>::Type>
{
    using Type = typename T::AllMessages;
};

template <class T>
using ProtocolLayerAllMessagesType = typename ProtocolLayerAllMessagesHelper<T>::Type;

template <typename T, bool THasImpl>
struct ProtocolLayerHasFieldsImplHelper;

template <typename T>
struct ProtocolLayerHasFieldsImplHelper<T, true>
{
    static const bool Value = T::ImplOptions::HasFieldsImpl;
};

template <typename T>
struct ProtocolLayerHasFieldsImplHelper<T, false>
{
    static const bool Value = false;
};

template <typename T>
struct ProtocolLayerHasFieldsImpl
{
    static const bool Value =
        ProtocolLayerHasFieldsImplHelper<T, comms::details::hasImplOptions<T>()>::Value;
};

template <class T>
constexpr bool protocolLayerHasFieldsImpl()
{
    return ProtocolLayerHasFieldsImpl<T>::Value;
}

template <typename T, bool THasImpl>
struct ProtocolLayerHasDoGetIdHelper;

template <typename T>
struct ProtocolLayerHasDoGetIdHelper<T, true>
{
    static const bool Value = T::ImplOptions::HasStaticMsgId;
};

template <typename T>
struct ProtocolLayerHasDoGetIdHelper<T, false>
{
    static const bool Value = false;
};

template <typename T>
struct ProtocolLayerHasDoGetId
{
    static const bool Value =
        ProtocolLayerHasDoGetIdHelper<T, comms::details::hasImplOptions<T>()>::Value;
};

template <typename T>
constexpr bool protocolLayerHasDoGetId()
{
    return ProtocolLayerHasDoGetId<T>::Value;
}

template <class T, class R = void>
struct ProtocolLayerEnableIfHasMsgPtr { using Type = R; };

template <class T, class Enable = void>
struct ProtocolLayerMsgPtr
{
    using Type = void;
};

template <class T>
struct ProtocolLayerMsgPtr<T, typename ProtocolLayerEnableIfHasMsgPtr<typename T::MsgPtr>::Type>
{
    using Type = typename T::MsgPtr;
};

template<typename...>
class MissingSizeRetriever
{
public:
    MissingSizeRetriever(std::size_t& val) : value_(val) {}

    void setValue(std::size_t val)
    {
        value_ = val;
    }

private:
    std::size_t& value_;
};

template <typename T>
struct IsMissingSizeRetrieverHelper
{
    static const bool Value = false;
};

template <typename... TParams>
struct IsMissingSizeRetrieverHelper<MissingSizeRetriever<TParams...> >
{
    static const bool Value = true;
};

template <typename T>
constexpr bool isMissingSizeRetriever()
{
    return IsMissingSizeRetrieverHelper<T>::Value;
}

template <typename TId>
class MsgIdRetriever
{
public:
    MsgIdRetriever(TId& val) : value_(val) {}

    template <typename U>
    void setValue(U&& val)
    {
        value_ = static_cast<TId>(val);
    }

private:
    TId& value_;
};

template <typename T>
struct IsMsgIdRetrieverHelper
{
    static const bool Value = false;
};

template <typename TId>
struct IsMsgIdRetrieverHelper<MsgIdRetriever<TId> >
{
    static const bool Value = true;
};

template <typename T>
constexpr bool isMsgIdRetriever()
{
    return IsMsgIdRetrieverHelper<T>::Value;
}

class MsgIndexRetriever
{
public:
    MsgIndexRetriever(std::size_t& val) : value_(val) {}

    void setValue(std::size_t val)
    {
        value_ = val;
    }

private:
    std::size_t& value_;
};

template <typename T>
struct IsMsgIndexRetrieverHelper
{
    static const bool Value = false;
};

template <>
struct IsMsgIndexRetrieverHelper<MsgIndexRetriever>
{
    static const bool Value = true;
};

template <typename T>
constexpr bool isMsgIndexRetriever()
{
    return IsMsgIndexRetrieverHelper<T>::Value;
}

template <typename TIter>
class MsgPayloadRetriever
{
public:
    MsgPayloadRetriever(TIter& iter, std::size_t& len) : iter_(iter), len_(len) {}

    template <typename TOtherIter>
    void setValue(TOtherIter iter, std::size_t len)
    {
        iter_ = static_cast<TIter>(iter);
        len_ = len;
    }

private:
    TIter& iter_;
    std::size_t& len_;
};

template <typename TITer>
struct IsMsgPayloadRetrieverHelper
{
    static const bool Value = false;
};

template <typename TIter>
struct IsMsgPayloadRetrieverHelper<MsgPayloadRetriever<TIter> >
{
    static const bool Value = true;
};

template <typename T>
constexpr bool isMsgPayloadRetriever()
{
    return IsMsgPayloadRetrieverHelper<T>::Value;
}

} // namespace details

} // namespace protocol

} // namespace comms
