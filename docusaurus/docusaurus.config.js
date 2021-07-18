const lightCodeTheme = require('prism-react-renderer/themes/github');
const darkCodeTheme = require('prism-react-renderer/themes/dracula');

/** @type {import('@docusaurus/types').DocusaurusConfig} */
module.exports = {
    title: 'Murmur',
    tagline: 'An experimental programming language for everyone.',
    url: 'https://murmur.org',
    baseUrl: '/murmur-test/',
    onBrokenLinks: 'throw',
    onBrokenMarkdownLinks: 'warn',
    favicon: 'img/favicon.ico',
    organizationName: 'EnderCommunity', // Usually your GitHub org/user name.
    projectName: 'Murmur', // Usually your repo name.
    themeConfig: {
        hideableSidebar: true,
        colorMode: {
            defaultMode: 'dark',
            disableSwitch: true,
            respectPrefersColorScheme: true,
        },
        navbar: {
            title: 'Murmur',
            logo: {
                alt: 'Mur\'s Logo',
                src: 'img/logo.svg',
            },
            items: [{
                    type: 'doc',
                    docId: 'intro',
                    position: 'left',
                    label: 'Docs',
                },
                {
                    label: 'Tools & Downloads',
                    position: 'left',
                    to: '/soon/',
                },
                //Search Bar
                {
                    label: 'Playground',
                    position: 'left',
                    to: '/soon/',
                },
                {
                    to: '/blog',
                    label: 'Blog',
                    position: 'left',
                },
                {
                    label: 'Help',
                    position: 'left',
                    to: '/soon/',
                },
                {
                    type: 'localeDropdown',
                    position: 'right',
                    dropdownItemsAfter: [{
                        href: '/soon/',
                        label: 'Help Us Translate',
                    }, ],
                },
                {
                    type: 'docsVersionDropdown',
                    position: 'right',
                    dropdownItemsAfter: [{
                        href: '/soon/',
                        label: 'All Versions',
                    }, ],
                },
                {
                    href: 'https://github.com/EnderCommunity/Murmur',
                    'aria-label': 'GitHub Repository',
                    position: 'right',
                    className: 'header-github-link',
                },
            ],
        },
        footer: {
            style: 'dark',
            links: [{
                    title: 'Get Started',
                    items: [{
                            label: 'Docs',
                            to: '/docs/intro',
                        },
                        {
                            label: 'Tools & Downloads',
                            href: '/soon/',
                        },
                        {
                            label: 'Playground',
                            href: '/soon/',
                        },
                    ],
                },
                {
                    title: 'Community',
                    items: [{
                            label: 'Twitter',
                            href: 'https://twitter.com/_EnderCommunity',
                        },
                        {
                            label: 'Discord',
                            href: 'https://discord.gg/aWBnkJpgzt',
                        },
                        {
                            label: 'Stack Overflow',
                            href: 'https://stackoverflow.com/questions/tagged/mur',
                        },
                    ],
                },
                {
                    title: 'More',
                    items: [{
                            label: 'Blog',
                            to: '/blog',
                        },
                        {
                            label: 'GitHub',
                            href: 'https://github.com/EnderCommunity/Murmur',
                        },
                        {
                            label: 'EnderCommunity',
                            href: 'https://endercomm.net',
                        },
                    ],
                },
                {
                    title: 'Legal',
                    // Please do not remove the privacy and terms, it's a legal requirement.
                    items: [{
                            label: 'Privacy',
                            href: '/soon/',
                        },
                        {
                            label: 'Terms',
                            href: '/soon/',
                        },
                        {
                            label: 'Data Policy',
                            href: '/soon/',
                        },
                        {
                            label: 'Cookie Policy',
                            href: '/soon/',
                        },
                    ],
                },
            ],
            copyright: `<br>Made with 💜 in Palestine | Copyright © ${new Date().getFullYear()} EnderCommunity`,
        },
        prism: {
            theme: lightCodeTheme,
            darkTheme: darkCodeTheme,
        },
        announcementBar: {
            id: 'announcementBar',
            content: 'Please note that Mur is still in the pre-alpha development stages! (It\'s not functional yet)',
            isCloseable: false, // Defaults to `true`.
        },
    },
    presets: [
        [
            '@docusaurus/preset-classic',
            {
                docs: {
                    sidebarPath: require.resolve('./sidebars.js'),
                    // Please change this to your repo.
                    editUrl: 'https://github.com/EnderCommunity/Murmur/edit/main/docusaurus/',
                },
                docs: {
                    showLastUpdateAuthor: true,
                    showLastUpdateTime: true,
                    versions: {
                        current: {
                            label: `0.0-test.5 🚧`,
                        },
                    },
                },
                blog: {
                    showReadingTime: true,
                    // Please change this to your repo.
                    editUrl: 'https://github.com/EnderCommunity/Murmur/edit/main/docusaurus/',
                },
                theme: {
                    customCss: require.resolve('./src/css/custom.css'),
                },
            },
        ],
    ],
    i18n: {
        defaultLocale: 'en-GB',
        locales: ['en-GB', 'ar'],
    },
};