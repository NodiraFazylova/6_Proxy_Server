void CommandLine::addArgument( std::vector<std::string> const & flags,
                               Value const & value, std::string const & help )
{
    mArguments.emplace_back( Argument{ flags, value, help } );
}

void CommandLine::printHelp( std::ostream & os ) const
{

    // Print the general description.
    os << "Using: " << mDescription << " [option]" << std::endl;

    // Find the argument with the longest combined flag length (in order
    // to align the help messages).

    uint32_t maxFlagLength = 0;

    for( auto const & argument : mArguments )
    {
        uint32_t flagLength = 0;
        for( auto const & flag : argument.mFlags )
        {
            // Plus comma and space.
            flagLength += static_cast<uint32_t>(flag.size()) + 2;
        }

        maxFlagLength = std::max( maxFlagLength, flagLength );
    }

    // Now print each argument.
    for( auto const & argument : mArguments )
    {

        std::string flags;
        for( auto const & flag : argument.mFlags )
        {
            flags += flag + ", ";
        }

        // Remove last comma and space and add padding according to the
        // longest flags in order to align the help messages.
        std::stringstream sstr;
        sstr << std::left << std::setw( maxFlagLength )
            << flags.substr( 0, flags.size() - 2 );

        // Print the help for each argument. This is a bit more involved
        // since we do line wrapping for long descriptions.
        size_t spacePos = 0;
        size_t lineWidth = 0;
        while( spacePos != std::string::npos )
        {
            size_t nextspacePos = argument.mHelp.find_first_of( ' ', spacePos + 1 );
            sstr << argument.mHelp.substr( spacePos, nextspacePos - spacePos );
            lineWidth += nextspacePos - spacePos;
            spacePos = nextspacePos;

            if( lineWidth > 55 )
            {
                os << sstr.str() << std::endl;
                sstr = std::stringstream();
                sstr << std::left << std::setw( maxFlagLength - 1 ) << " ";
                lineWidth = 0;
            }
        }
    }
}

void CommandLine::parse( int argc, char * argv[] ) const
{
    mDescription = argv[0];
#ifdef _WIN32
    mDescription = mDescription.substr( mDescription.find_last_of( "\\" ) + 1 );
#else
    mDescription = mDescription.substr( mDescription.find_last_of( "/" ) + 1 );
#endif

    if( argc < 2 )
    {
        set_help_opt_if();
    }

    // Skip the first argument (name of the program).
    int i = 1;
    while( i < argc )
    {

        // First we have to identify wether the value is separated by a space
        // or a '='.
        std::string flag( argv[i] );
        std::string value;
        bool        valueIsSeparate = false;

        // If there is an '=' in the flag, the part after the '=' is actually
        // the value.
        size_t equalPos = flag.find( '=' );
        if( equalPos != std::string::npos )
        {
            value = flag.substr( equalPos + 1 );
            flag = flag.substr( 0, equalPos );
        }
        // Else the following argument is the value.
        else if( i + 1 < argc )
        {
            value = argv[i + 1];
            valueIsSeparate = true;
        }

        // Search for an argument with the provided flag.
        bool foundArgument = false;

        for( auto const & argument : mArguments )
        {
            if( std::find( argument.mFlags.begin(), argument.mFlags.end(), flag )
                != std::end( argument.mFlags ) )
            {

                foundArgument = true;

                // In the case of booleans, there must not be a value present.
                // So if the value is neither 'true' nor 'false' it is considered
                // to be the next argument.
                if( std::holds_alternative<bool *>( argument.mValue ) )
                {
                    if( !value.empty() && value != "true" && value != "false" )
                    {
                        valueIsSeparate = false;
                    }
                    *std::get<bool *>( argument.mValue ) = (value != "false");
                }
                // In all other cases there must be a value.
                else if( value.empty() )
                {
                   LOG_ERROR("CommandLine::addArgument() - Failed to parse command line arguments,"
                               "missing value for argument \"{0}\"", flag );

                   set_help_opt_if();
                }
                // For a std::string, we take the entire value.
                else if( std::holds_alternative<std::string *>( argument.mValue ) )
                {
                    *std::get<std::string *>( argument.mValue ) = value;
                }
                // In all other cases we use a std::stringstream to
                // convert the value.
                else
                {
                    std::visit(
                        [&value] ( auto && arg ) {
                        std::stringstream sstr( value );
                        sstr >> *arg;
                    },
                        argument.mValue );
                }

                break;
            }
        }

        // Print a warning if there was an unknown argument.
        if( !foundArgument )
        {
            LOG_ERROR("CommandLine::addArgument() - Ignoring unknown command line argument \"{0}\"", flag );

            set_help_opt_if();
        }

        // Advance to the next flag.
        ++i;

        // If the value was separated, we have to advance our index once more.
        if( foundArgument && valueIsSeparate )
        {
            ++i;
        }
    }
}


void CommandLine::set_help_opt_if() const
{
    auto pos = std::begin( mArguments );
    for( pos; pos != std::end( mArguments ); pos = std::next( pos ) )
    {
        if( std::find( std::begin( pos->mFlags ), std::end( pos->mFlags ), "-h" ) != std::end( pos->mFlags ) ||
            std::find( std::begin( pos->mFlags ), std::end( pos->mFlags ), "-?" ) != std::end( pos->mFlags ) ||
            std::find( std::begin( pos->mFlags ), std::end( pos->mFlags ), "--help" ) != std::end( pos->mFlags ) )
        {
            *std::get<bool *>( pos->mValue ) = true;
            break;
        }
    }
}
