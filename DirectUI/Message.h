#pragma once

namespace directui
{

class Message
{
private:
	const char* _name;
public:
	Message( const char* name ) : _name{ name } {}
	~Message() {}

	template< typename TMessage >
	const TMessage* As() const
	{
		if ( TMessage::Name() == _name )
		{
			return static_cast< const TMessage* >( this );
		}
		return nullptr;
	}
};

} // namespace directui
