/* ============================================================
   HPSTREAM — Next-Gen 3D Movie Streaming Web Engine
   Clean Architecture, Fast Loading, Bug-Free, Real 3D Interactivity
   ============================================================ */

(() => {
  'use strict';

  /* ============================================================
     GENRES & CATEGORIES CONFIGURATION
     ============================================================ */
  const GENRES = [
    { name: 'Action', c1: '#3a0d12', c2: '#8a1a26', img: 'https://images.unsplash.com/photo-1536440136628-849c177e76a1?w=600&auto=format&fit=crop&q=80' },
    { name: 'Adventure', c1: '#0d2b3a', c2: '#15607f', img: 'https://images.unsplash.com/photo-1518709268805-4e9042af9f23?w=600&auto=format&fit=crop&q=80' },
    { name: 'Comedy', c1: '#3a2b0d', c2: '#8a6614', img: 'https://images.unsplash.com/photo-1514306191717-452ec28c7814?w=600&auto=format&fit=crop&q=80' },
    { name: 'Drama', c1: '#190f2e', c2: '#3f2166', img: 'https://images.unsplash.com/photo-1485846234645-a62644f84728?w=600&auto=format&fit=crop&q=80' },
    { name: 'Horror', c1: '#0a0a0a', c2: '#33090c', img: 'https://images.unsplash.com/photo-1509248961158-e54f6934749c?w=600&auto=format&fit=crop&q=80' },
    { name: 'Thriller', c1: '#0d1a2b', c2: '#1f4066', img: 'https://images.unsplash.com/photo-1509347528160-9a9e33742cdb?w=600&auto=format&fit=crop&q=80' },
    { name: 'Romance', c1: '#3a0d24', c2: '#8a1458', img: 'https://images.unsplash.com/photo-1518199266791-5375a83190b7?w=600&auto=format&fit=crop&q=80' },
    { name: 'Sci-Fi', c1: '#0d2b2b', c2: '#146666', img: 'https://images.unsplash.com/photo-1451187580459-43490279c0fa?w=600&auto=format&fit=crop&q=80' },
    { name: 'Fantasy', c1: '#1f0d3a', c2: '#521c8a', img: 'https://images.unsplash.com/photo-1518709268805-4e9042af9f23?w=600&auto=format&fit=crop&q=80' },
    { name: 'Animation', c1: '#0d3a2b', c2: '#158a5f', img: 'https://images.unsplash.com/photo-1534447677768-be436bb09401?w=600&auto=format&fit=crop&q=80' },
    { name: 'Crime', c1: '#14141a', c2: '#40151b', img: 'https://images.unsplash.com/photo-1478720568477-152d9b164e26?w=600&auto=format&fit=crop&q=80' },
    { name: 'Mystery', c1: '#10121e', c2: '#242b52', img: 'https://images.unsplash.com/photo-1518709268805-4e9042af9f23?w=600&auto=format&fit=crop&q=80' },
    { name: 'Documentary', c1: '#1a1a1a', c2: '#3d372c', img: 'https://images.unsplash.com/photo-1485846234645-a62644f84728?w=600&auto=format&fit=crop&q=80' }
  ];

  const gmap = {};
  GENRES.forEach(g => { gmap[g.name] = g; });

  const CATS = [
    { key: 'all', label: 'All' },
    { key: 'hollywood', label: 'Hollywood' },
    { key: 'bollywood', label: 'Bollywood' },
    { key: 'anime', label: 'Anime' },
    { key: 'kdrama', label: 'K-Drama' },
    { key: 'webseries', label: 'Web Series' }
  ];

  function ep(n, title, dur, desc, still) {
    return { ep: n, title, dur, desc, still: still || '' };
  }

  /* ============================================================
     CURATED HIGH-DEFINITION MASTER DATASET
     ============================================================ */
  const INITIAL_TITLES = [
    {
      id: 'midnight-horizon',
      title: 'Midnight Horizon',
      tmdbId: '603', // Matrix example
      tmdbId: '603',
      type: 'movie',
      category: 'hollywood',
      tagline: 'When the sun sets, the truth rises.',
      rating: 8.8,
      year: 2026,
      duration: '2h 18m',
      age: 'U/A 16+',
      languages: ['English', 'Hindi'],
      genres: ['Action', 'Thriller', 'Crime'],
      quality: ['4K', 'HDR', '5.1'],
      cast: ['Adrian Cole', 'Nina Marsh', 'Owen Blake', 'Priya Anand'],
      director: 'Marcus Feld',
      description: 'A disgraced detective has one night to stop a citywide blackout scheme before it swallows everything he has left to protect. As the power grid goes pitch black block by block, old debts and syndicate enforcers surface across neon-lit skyscrapers.',
      poster: 'https://images.unsplash.com/photo-1536440136628-849c177e76a1?w=800&auto=format&fit=crop&q=85',
      backdrop: 'https://images.unsplash.com/photo-1509198397868-475647b2a1e5?w=1600&auto=format&fit=crop&q=85',
      youtubeTrailer: 'https://www.youtube.com/embed/dQw4w9WgXcQ?autoplay=1',
      isTrending: true,
      isNew: true,
      trailerLen: '2:14',
      teaserLen: '0:42'
    },
    {
      id: 'rangrezz',
      title: 'Rangrezz: Symphony of Fire',
      tmdbId: '557', // Spider-man example
      tmdbId: '557',
      tmdbId: '557',
      type: 'movie',
      category: 'bollywood',
      tagline: 'Every colour tells a story of rebellion and love.',
      rating: 8.6,
      year: 2026,
      duration: '2h 32m',
      age: 'U/A 13+',
      languages: ['Hindi', 'English', 'Tamil'],
      genres: ['Drama', 'Romance', 'Action'],
      quality: ['4K', 'Dolby Atmos'],
      cast: ['Aarav Malhotra', 'Simran Kapoor', 'Devika Rao', 'Pankaj Tripathi'],
      director: 'Kabir Suri',
      description: 'A master textile artisan in the blue lanes of old Jodhpur discovers an underground heritage smuggling ring backed by powerful royal oligarchs, igniting a fiery revolution to save the city\'s ancestral soul.',
      poster: 'https://images.unsplash.com/photo-1518709268805-4e9042af9f23?w=800&auto=format&fit=crop&q=85',
      backdrop: 'https://images.unsplash.com/photo-1506744038136-46273834b3fb?w=1600&auto=format&fit=crop&q=85',
      youtubeTrailer: 'https://www.youtube.com/embed/L_LUpnjgPso?autoplay=1',
      isTrending: true,
      isNew: false,
      trailerLen: '2:30',
      teaserLen: '0:48'
    },
    {
      id: 'kage-no-kishi',
      title: 'Kage no Kishi: Shadow Knight',
      tmdbId: '94605',
      tmdbId: '94605',
      tmdbId: '94605', // Anime example
      type: 'series',
      category: 'anime',
      tagline: 'The Shadow Knight remembers every lost timeline.',
      rating: 9.1,
      year: 2026,
      age: 'U/A 16+',
      languages: ['Japanese', 'English', 'Hindi'],
      genres: ['Animation', 'Action', 'Fantasy', 'Sci-Fi'],
      quality: ['4K', 'SUB', 'DUB'],
      cast: ['Voice: Ren Katsuragi', 'Voice: Miu Fujita', 'Voice: Kenjiro Tsuda'],
      director: 'Studio Kurogane',
      description: 'A cursed swordsman bound to relive every fatal battle finds a temporal blade that retains memory across loops. In the neon-cyber feudal capital of Neo-Edo, he challenges immortal deities to preserve his fallen clan.',
      poster: 'https://images.unsplash.com/photo-1578632767115-351597cf2477?w=800&auto=format&fit=crop&q=85',
      backdrop: 'https://images.unsplash.com/photo-1563089145-599997674d42?w=1600&auto=format&fit=crop&q=85',
      youtubeTrailer: 'https://www.youtube.com/embed/1roy4o4tqQM?autoplay=1',
      isTrending: true,
      isNew: true,
      trailerLen: '1:45',
      teaserLen: '0:35',
      seasons: [
        {
          season: 1,
          episodes: [
            ep(1, 'The First Loop: Blood & Petals', '24m', 'Kirei wakes on the rain-soaked battlefield for the forty-seventh time with a shattered blade.'),
            ep(2, 'The Ashen Prince', '23m', 'A shadow merchant reveals the secret bloodline behind the Emperor\'s endless mechanical army.'),
            ep(3, 'Blade of the Forgotten Gods', '25m', 'Kirei infiltrates the Grand Clocktower to recover the catalyst before midnight.'),
            ep(4, 'Rain Over Kurogane', '24m', 'The orbital defense barrier is breached, unleashing catastrophic temporal rifts.'),
            ep(5, 'The Unbroken Timeline', '26m', 'A pivotal choice rewrites fate, opening the gateway to the outer realm.')
          ]
        },
        {
          season: 2,
          episodes: [
            ep(1, 'The Waking Citadel', '24m', 'A rejuvenated kingdom awakens to a new menace from the cosmic horizon.'),
            ep(2, 'Iron Court Intrigue', '23m', 'Political diplomacy proves deadlier than the edge of any temporal sword.'),
            ep(3, 'The Singularity Oath', '25m', 'Kirei and Miu form an unyielding alliance to save the celestial core.')
          ]
        }
      ]
    },
    {
      id: 'winter-sonata-redux',
      title: 'Winter Sonata Redux',
      type: 'series',
      category: 'kdrama',
      tagline: 'Some melodies echo through decades of snow.',
      rating: 8.9,
      year: 2026,
      age: 'U/A 13+',
      languages: ['Korean', 'English', 'Hindi'],
      genres: ['Drama', 'Romance'],
      quality: ['4K', 'Dolby Vision', 'SUB'],
      cast: ['Ji-ho Baek', 'Soo-ah Lim', 'Tae-yang Moon', 'Han So-hee'],
      director: 'Han Seo-yeon',
      description: 'A world-renowned concert pianist returns to her snowbound alpine hometown after a decade of absence, discovering that her high school love now operates the mountain\'s oldest sanctuary inn.',
      poster: 'https://images.unsplash.com/photo-1518199266791-5375a83190b7?w=800&auto=format&fit=crop&q=85',
      backdrop: 'https://images.unsplash.com/photo-1483921020237-2ff51e8e4b22?w=1600&auto=format&fit=crop&q=85',
      youtubeTrailer: 'https://www.youtube.com/embed/kJQP7kiw5Fk?autoplay=1',
      isTrending: true,
      isNew: false,
      trailerLen: '1:50',
      teaserLen: '0:38',
      seasons: [
        {
          season: 1,
          episodes: [
            ep(1, 'First Snowfall in Pyeongchang', '62m', 'Soo-ah checks into Room 4 without recognizing the innkeeper who once wrote her first concerto.'),
            ep(2, 'The Silent Piano in the Attic', '58m', 'An unfinished music sheet reveals secrets kept hidden during ten years of separation.'),
            ep(3, 'The Blizzard of 2016', '60m', 'A historic snowstorm strands the travelers, forcing forgotten memories into the open.')
          ]
        }
      ]
    },
    {
      id: 'velocity',
      title: 'Velocity: Chrono Surge',
      type: 'movie',
      category: 'hollywood',
      tagline: 'Outrun the clock before time runs out for humanity.',
      rating: 8.5,
      year: 2026,
      duration: '1h 58m',
      age: 'U/A 16+',
      languages: ['English', 'Hindi', 'German'],
      genres: ['Sci-Fi', 'Action', 'Thriller'],
      quality: ['4K', 'HDR10+'],
      cast: ['Owen Blake', 'Rhea Sato', 'Tomas King'],
      director: 'Wren Achebe',
      description: 'In a futuristic metropolis where human lifespan is traded as digital currency, an elite underground runner intercepts a drive containing unlimited time credits.',
      poster: 'https://images.unsplash.com/photo-1451187580459-43490279c0fa?w=800&auto=format&fit=crop&q=85',
      backdrop: 'https://images.unsplash.com/photo-1518709268805-4e9042af9f23?w=1600&auto=format&fit=crop&q=85',
      youtubeTrailer: 'https://www.youtube.com/embed/yoPeAms42t4?autoplay=1',
      isTrending: true,
      isNew: true,
      trailerLen: '2:05',
      teaserLen: '0:45'
    },
    {
      id: 'steel-requiem',
      title: 'Steel Requiem',
      type: 'movie',
      category: 'hollywood',
      tagline: 'Loyalty has a price. Vengeance is priceless.',
      rating: 8.2,
      year: 2026,
      duration: '2h 11m',
      age: 'A 18+',
      languages: ['English', 'Hindi'],
      genres: ['Action', 'Crime', 'Thriller'],
      quality: ['4K', 'HD'],
      cast: ['Tomas King', 'Adrian Cole', 'Rhea Sato'],
      director: 'Ines Calder',
      description: 'A retired special operative is summoned back to the underworld when his former brotherhood orchestrates an international corporate takeover.',
      poster: 'https://images.unsplash.com/photo-1478720568477-152d9b164e26?w=800&auto=format&fit=crop&q=85',
      backdrop: 'https://images.unsplash.com/photo-1509347528160-9a9e33742cdb?w=1600&auto=format&fit=crop&q=85',
      youtubeTrailer: 'https://www.youtube.com/embed/tgbNymZ7vqY?autoplay=1',
      isTrending: false,
      isNew: true,
      trailerLen: '2:10',
      teaserLen: '0:40'
    },
    {
      id: 'seoul-undercover',
      title: 'Seoul Undercover',
      type: 'series',
      category: 'kdrama',
      tagline: 'Trust no badge. The syndicate is everywhere.',
      rating: 8.7,
      year: 2025,
      age: 'U/A 16+',
      languages: ['Korean', 'English'],
      genres: ['Crime', 'Thriller', 'Mystery', 'Action'],
      quality: ['4K', 'SUB'],
      cast: ['Tae-yang Moon', 'Eun-ji Park', 'Ji-ho Baek'],
      director: 'Choi Min-jae',
      description: 'An undercover intelligence agent infiltrates an elite maritime syndicate just as internal affairs opens a treason investigation into her covert team.',
      poster: 'https://images.unsplash.com/photo-1509347528160-9a9e33742cdb?w=800&auto=format&fit=crop&q=85',
      backdrop: 'https://images.unsplash.com/photo-1514565131-fce0801e5785?w=1600&auto=format&fit=crop&q=85',
      youtubeTrailer: 'https://www.youtube.com/embed/L_LUpnjgPso?autoplay=1',
      isTrending: true,
      isNew: false,
      trailerLen: '2:00',
      teaserLen: '0:46',
      seasons: [
        {
          season: 1,
          episodes: [
            ep(1, 'Infiltration Point', '55m', 'Eun-ji establishes her undercover identity inside the Port District hierarchy.'),
            ep(2, 'The Double Ledger', '57m', 'A cryptographic key proves corruption within the Police Commissioner\'s inner circle.')
          ]
        }
      ]
    },
    {
      id: 'firewall',
      title: 'Firewall: Cyber Heist',
      type: 'series',
      category: 'webseries',
      tagline: 'Every system has a zero-day exploit.',
      rating: 8.3,
      year: 2026,
      age: 'U/A 16+',
      languages: ['English', 'Hindi'],
      genres: ['Thriller', 'Crime', 'Sci-Fi'],
      quality: ['4K', 'HD'],
      cast: ['Priya Anand', 'Wren Achebe', 'Faisal Rhone'],
      director: 'Dana Okafor',
      description: 'A rogue security architect discovers an artificial intelligence backdoor implanted deep into global banking infrastructure.',
      poster: 'https://images.unsplash.com/photo-1526374965328-7f61d4dc18c5?w=800&auto=format&fit=crop&q=85',
      backdrop: 'https://images.unsplash.com/photo-1558494949-ef010cbdcc31?w=1600&auto=format&fit=crop&q=85',
      youtubeTrailer: 'https://www.youtube.com/embed/1roy4o4tqQM?autoplay=1',
      isTrending: true,
      isNew: true,
      trailerLen: '1:40',
      teaserLen: '0:32',
      seasons: [
        {
          season: 1,
          episodes: [
            ep(1, 'Zero Day Protocol', '38m', 'Priya uncovers code that executes commands outside the central cloud.'),
            ep(2, 'The Air Gap Defense', '36m', 'The team isolates a quantum server in an abandoned bunker.')
          ]
        }
      ]
    }
  ];

  /* Active dataset & lookup */
  let TITLES = [...INITIAL_TITLES];
  const byId = {};
  function rebuildIndex() {
    Object.keys(byId).forEach(k => delete byId[k]);
    TITLES.forEach(t => { byId[t.id] = t; });
  }
  rebuildIndex();

  function primaryGenreColor(item) {
    const g = (item && item.genres && item.genres[0]) || 'Action';
    return gmap[g] || GENRES[0];
  }

  /* ============================================================
     PERSISTENCE & STATE MANAGEMENT
     ============================================================ */
  const STORAGE_KEYS = {
    WATCHLIST: 'hpstream_watchlist_v2',
    HISTORY: 'hpstream_history_v2',
    USER: 'hpstream_user_v2'
  };

  function loadSavedWatchlist() {
    try {
      const data = localStorage.getItem(STORAGE_KEYS.WATCHLIST);
      return data ? new Set(JSON.parse(data)) : new Set(['seoul-undercover', 'midnight-horizon', 'kage-no-kishi']);
    } catch {
      return new Set(['seoul-undercover', 'midnight-horizon']);
    }
  }

  function saveWatchlist() {
    try {
      localStorage.setItem(STORAGE_KEYS.WATCHLIST, JSON.stringify(Array.from(state.watchlist)));
    } catch (e) {
      console.warn('Watchlist storage failed:', e);
    }
  }

  function loadSavedHistory() {
    try {
      const data = localStorage.getItem(STORAGE_KEYS.HISTORY);
      return data ? JSON.parse(data) : [
        { id: 'midnight-horizon', pct: 74, label: '1h 42m watched' },
        { id: 'seoul-undercover', pct: 52, label: 'S1 · E2' },
        { id: 'rangrezz', pct: 88, label: '2h 10m watched' },
        { id: 'kage-no-kishi', pct: 35, label: 'S2 · E1' }
      ];
    } catch {
      return [];
    }
  }

  function saveHistoryItem(id, pct, label) {
    try {
      const list = loadSavedHistory().filter(h => h.id !== id);
      list.unshift({ id, pct: Math.min(100, Math.max(1, Math.round(pct))), label });
      localStorage.setItem(STORAGE_KEYS.HISTORY, JSON.stringify(list.slice(0, 12)));
    } catch (e) {
      console.warn('History storage failed:', e);
    }
  }

  const state = {
    view: 'home',
    params: {},
    watchlist: loadSavedWatchlist(),
    history: loadSavedHistory(),
    isLoggedIn: !!localStorage.getItem(STORAGE_KEYS.USER),
    heroIndex: 0,
    heroTimer: null,
    popularCat: 'hollywood',
    playerPlaying: false,
    playerPct: 15,
    playerTimer: null,
    activeFilters: {}
  };

  /* ============================================================
     ICONS (OPTIMIZED SVG)
     ============================================================ */
  const ic = {
    search: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round"><circle cx="11" cy="11" r="7"/><path d="M21 21l-4.3-4.3"/></svg>`,
    heart: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round"><path d="M20.8 4.6c-1.9-1.9-5-1.9-6.9 0L12 5.5l-1.9-1a4.9 4.9 0 0 0-6.9 6.9L12 20.3l8.8-8.9c1.9-1.9 1.9-5 0-6.9Z"/></svg>`,
    user: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><circle cx="12" cy="8" r="4"/><path d="M4 21c1.6-4 5-6 8-6s6.4 2 8 6"/></svg>`,
    play: `<svg viewBox="0 0 24 24" fill="currentColor"><path d="M8 5v14l11-7L8 5Z"/></svg>`,
    pause: `<svg viewBox="0 0 24 24" fill="currentColor"><path d="M7 5h4v14H7zm6 0h4v14h-4z"/></svg>`,
    chevL: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round" stroke-linejoin="round"><path d="M15 18l-6-6 6-6"/></svg>`,
    chevR: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round" stroke-linejoin="round"><path d="M9 18l6-6-6-6"/></svg>`,
    plus: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round"><path d="M12 5v14M5 12h14"/></svg>`,
    check: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.6" stroke-linecap="round" stroke-linejoin="round"><path d="M20 6 9 17l-5-5"/></svg>`,
    close: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round"><path d="M18 6 6 18M6 6l12 12"/></svg>`,
    star: `<svg viewBox="0 0 24 24" fill="currentColor"><path d="M12 2.5l2.9 6.4 6.9.6-5.2 4.7 1.6 6.8L12 17.6l-6.2 3.4 1.6-6.8L2.2 9.5l6.9-.6L12 2.5Z"/></svg>`,
    vol: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><path d="M4 9v6h4l5 4V5L8 9H4Z"/><path d="M17.5 8.5a5 5 0 0 1 0 7"/></svg>`,
    expand: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><path d="M8 3H5a2 2 0 0 0-2 2v3M16 3h3a2 2 0 0 1 2 2v3M8 21H5a2 2 0 0 1-2-2v-3M16 21h3a2 2 0 0 0 2-2v-3"/></svg>`,
    gear: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.7 1.7 0 0 0 .3 1.9l.1.1a2 2 0 1 1-2.8 2.8l-.1-.1a1.7 1.7 0 0 0-1.9-.3 1.7 1.7 0 0 0-1 1.5V21a2 2 0 1 1-4 0v-.1a1.7 1.7 0 0 0-1-1.6 1.7 1.7 0 0 0-1.9.3l-.1.1a2 2 0 1 1-2.8-2.8l.1-.1a1.7 1.7 0 0 0 .3-1.9 1.7 1.7 0 0 0-1.5-1H3a2 2 0 1 1 0-4h.1a1.7 1.7 0 0 0 1.6-1 1.7 1.7 0 0 0-.3-1.9l-.1-.1a2 2 0 1 1 2.8-2.8l.1.1a1.7 1.7 0 0 0 1.9.3H9a1.7 1.7 0 0 0 1-1.5V3a2 2 0 1 1 4 0v.1a1.7 1.7 0 0 0 1 1.5 1.7 1.7 0 0 0 1.9-.3l.1-.1a2 2 0 1 1 2.8 2.8l-.1.1a1.7 1.7 0 0 0-.3 1.9V9a1.7 1.7 0 0 0 1.5 1H21a2 2 0 1 1 0 4h-.1a1.7 1.7 0 0 0-1.5 1Z"/></svg>`,
    share: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><circle cx="18" cy="5" r="3"/><circle cx="6" cy="12" r="3"/><circle cx="18" cy="19" r="3"/><path d="M8.6 13.5l6.8 3.9M15.4 6.6 8.6 10.5"/></svg>`,
    filter: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round"><path d="M4 6h16M7 12h10M10 18h4"/></svg>`,
    film: `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="3" y="4" width="18" height="16" rx="2"/><path d="M7 4v16M17 4v16M3 9h4M17 9h4M3 15h4M17 15h4"/></svg>`
  };

  function fmtRating(item) {
    return `<span class="star">${ic.star}</span> ${item.rating || '8.5'}`;
  }

  function esc(s) {
    return (s || '').toString().replace(/[&<>"']/g, m => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' })[m]);
  }

  /* ============================================================
     3D CARD RENDERER WITH SPECULAR LIGHT GLARE
     ============================================================ */
  function cardHTML(item) {
    if (!item) return '';
    const g = primaryGenreColor(item);
    const isNew = item.isNew;
    const is4K = (item.quality || []).includes('4K');
    const catLabel = item.type === 'series' ? 'SERIES' : 'FILM';
    const posterSrc = item.poster || g.img;

    return `
    <div class="card-3d-wrap" data-action="open-detail" data-id="${item.id}">
      <div class="card-3d">
        <div class="card-poster">
          <div class="poster-art grain" style="background-image:url('${posterSrc}');"></div>
          <div class="poster-vignette"></div>
          <div class="poster-badges">
            ${isNew ? '<span class="badge badge-new">NEW</span>' : ''}
            ${is4K ? '<span class="badge badge-4k">4K</span>' : ''}
            <span class="badge">${(item.quality && item.quality[0]) || 'HD'}</span>
          </div>
          <div class="poster-topinfo">
            <div class="poster-cat">${catLabel}</div>
          </div>
          <div class="card-hover-overlay">
            <div class="card-play">${ic.play}</div>
          </div>
          <div class="card-glare"></div>
        </div>
        <div class="card-meta">
          <div class="card-title" title="${esc(item.title)}">${esc(item.title)}</div>
          <div class="card-sub">
            ${fmtRating(item)}
            <span class="dot-sep"></span>
            <span>${item.year || '2026'}</span>
            <span class="dot-sep"></span>
            <span>${item.type === 'series' ? 'Series' : 'Movie'}</span>
          </div>
        </div>
      </div>
    </div>`;
  }

  function progressCardHTML(cw) {
    const item = byId[cw.id] || INITIAL_TITLES[0];
    const posterSrc = item.poster || primaryGenreColor(item).img;
    return `
    <div class="card-3d-wrap progress-card" data-action="open-detail" data-id="${item.id}">
      <div class="card-3d">
        <div class="card-poster">
          <div class="poster-art grain" style="background-image:url('${posterSrc}');"></div>
          <div class="poster-vignette"></div>
          <div class="card-hover-overlay">
            <div class="card-play">${ic.play}</div>
          </div>
          <div class="card-glare"></div>
        </div>
        <div class="card-meta">
          <div class="card-title">${esc(item.title)}${cw.label ? ` · <span style="color:var(--text-faint);">${cw.label}</span>` : ''}</div>
          <div class="progress-bar-wrap">
            <div class="progress-bar-fill" style="width:${cw.pct}%"></div>
          </div>
          <div class="progress-pct">
            <span>${cw.pct}% completed</span>
            <span>${100 - cw.pct}% left</span>
          </div>
          <button class="btn btn-primary btn-sm continue-btn" data-action="go-watch" data-id="${item.id}">
            ${ic.play} Resume
          </button>
        </div>
      </div>
    </div>`;
  }

  function railSection({ title, sub, items, seeAllAction, railId, cardFn }) {
    const renderer = cardFn || cardHTML;
    const safeItems = (items || []).filter(Boolean);
    if (!safeItems.length) return '';

    return `
    <section class="section reveal in-view">
      <div class="container">
        <div class="section-head">
          <div>
            <h2 class="disp section-title">${title}</h2>
            ${sub ? `<div class="section-sub">${sub}</div>` : ''}
          </div>
          ${seeAllAction ? `<a class="seeall" data-action="${seeAllAction}">Explore All ${ic.chevR}</a>` : ''}
        </div>
        <div class="rail-wrap">
          <button class="rail-arrow left" data-action="rail-scroll" data-target="${railId}" data-dir="-1" aria-label="scroll left">${ic.chevL}</button>
          <div class="rail hide-scroll" id="${railId}">
            ${safeItems.map(renderer).join('')}
          </div>
          <button class="rail-arrow right" data-action="rail-scroll" data-target="${railId}" data-dir="1" aria-label="scroll right">${ic.chevR}</button>
        </div>
      </div>
    </section>`;
  }

  /* ============================================================
     HEADER & NAVIGATION (3D FROSTED GLASS)
     ============================================================ */
  function headerHTML() {
    const wl = state.watchlist.size;
    const navLinks = [
      { key: 'all', label: 'ALL' },
      { key: 'hollywood', label: 'HOLLYWOOD' },
      { key: 'bollywood', label: 'BOLLYWOOD' },
      { key: 'anime', label: 'ANIME' },
      { key: 'kdrama', label: 'K-DRAMA' },
      { key: 'webseries', label: 'WEB SERIES' },
      { key: 'genres', label: 'GENRES' }
    ];

    const currentCat = (state.view === 'browse' && state.params.category) ? state.params.category : (state.view === 'home' ? 'all' : '');

    return `
    <div class="header-inner container">
      <a class="logo" data-action="go-home" href="#">HPSTREAM<span class="dot">.</span></a>
      <nav class="nav-main">
        ${navLinks.map(n => `<a href="#" class="${currentCat === n.key ? 'active' : ''}" data-action="nav-cat" data-cat="${n.key}">${n.label}</a>`).join('')}
      </nav>
      <div class="header-right">
        <button class="icon-btn" data-action="open-search" aria-label="Search" title="Search (Ctrl+K)">${ic.search}</button>
        <button class="icon-btn watchlist-badge ${state.view === 'watchlist' ? 'active' : ''}" data-action="go-watchlist" aria-label="Watchlist" title="My Watchlist">
          ${ic.heart}${wl > 0 ? `<span class="count">${wl}</span>` : ''}
        </button>
        <button class="icon-btn ${state.view === 'profile' ? 'active' : ''}" data-action="profile-click" aria-label="Profile" title="Profile">
          ${ic.user}
        </button>
        <button class="icon-btn mobile-menu-btn" data-action="open-mobile-nav" aria-label="Menu">
          <span class="hamburger-lines"><span></span><span></span><span></span></span>
        </button>
      </div>
    </div>`;
  }

  function mobileNavHTML() {
    const navLinks = [
      { key: 'all', label: 'All Titles' },
      { key: 'hollywood', label: 'Hollywood Movies' },
      { key: 'bollywood', label: 'Bollywood Cinema' },
      { key: 'anime', label: 'Anime Series' },
      { key: 'kdrama', label: 'K-Drama' },
      { key: 'webseries', label: 'Web Series' },
      { key: 'genres', label: 'Browse Genres' }
    ];
    return `
      <div style="display:flex;align-items:center;justify-content:space-between;">
        <div class="logo">HPSTREAM<span class="dot">.</span></div>
        <button class="icon-btn" data-action="close-mobile-nav" aria-label="Close">${ic.close}</button>
      </div>
      <div class="mobile-nav-links">
        ${navLinks.map(n => `<a href="#" data-action="nav-cat" data-cat="${n.key}">${n.label} ${ic.chevR}</a>`).join('')}
      </div>
      <div style="display:flex;flex-direction:column;gap:12px;margin-top:auto;">
        <button class="btn btn-primary" data-action="go-watchlist">${ic.heart} My Watchlist (${state.watchlist.size})</button>
        <button class="btn btn-ghost" data-action="profile-click">${ic.user} Account Settings</button>
      </div>
    `;
  }

  /* ============================================================
     HERO SECTION (3D PARALLAX & AMBIENT CANVAS)
     ============================================================ */
  function heroSlideHTML(item, idx) {
    if (!item) return '';
    const g = primaryGenreColor(item);
    const active = idx === state.heroIndex ? 'active' : '';
    const catLabel = CATS.find(c => c.key === item.category)?.label || 'Cinema Blockbuster';
    const bgUrl = item.backdrop || item.poster || g.img;
    const posterUrl = item.poster || g.img;
    const inWL = state.watchlist.has(item.id);

    return `
    <div class="hero-slide ${active}" data-idx="${idx}">
      <div class="hero-bg grain" style="background-image:url('${bgUrl}');"></div>
      <div class="container hero-content">
        <div class="hero-left">
          <div class="hero-badge-row">
            <span class="pill pill-red">${item.isTrending ? 'Trending #1' : 'Featured Premiere'}</span>
            <span class="pill pill-line">${catLabel}</span>
            <span class="pill pill-line">4K ULTRA HD</span>
          </div>
          <h1 class="disp hero-title">${esc(item.title)}</h1>
          <p class="hero-desc">${esc(item.tagline || item.description.split('.')[0] + '.')}</p>
          <div class="hero-meta">
            <span class="rating">${ic.star} ${item.rating || '8.8'}</span>
            <span class="dot-sep"></span>
            <span>${item.year || '2026'}</span>
            <span class="dot-sep"></span>
            <span>${item.age || 'U/A 16+'}</span>
            <span class="dot-sep"></span>
            <span>${(item.languages || ['English']).join(' · ')}</span>
            ${item.duration ? `<span class="dot-sep"></span><span>${item.duration}</span>` : ''}
          </div>
          <div class="tag-row">
            ${(item.genres || ['Action']).map(g2 => `<span class="tag">${g2}</span>`).join('')}
          </div>
          <div class="hero-btn-row">
            <button class="btn btn-primary" data-action="go-watch" data-id="${item.id}">
              ${ic.play} Watch Now
            </button>
            <button class="btn btn-ghost" data-action="open-trailer" data-id="${item.id}" data-kind="trailer">
              ${ic.play} Trailer
            </button>
            <button class="btn btn-outline" data-action="toggle-watchlist" data-id="${item.id}">
              ${inWL ? ic.check + ' In Watchlist' : ic.plus + ' Watchlist'}
            </button>
          </div>
        </div>

        <div class="hero-right">
          <div class="poster-3d-frame">
            <div class="poster-art grain" style="background-image:url('${posterUrl}');"></div>
            <div class="poster-vignette"></div>
            <div class="card-glare"></div>
          </div>

          <div class="circle-badge">
            <svg viewBox="0 0 154 154" class="spin">
              <defs><path id="circPath" d="M77,14 a63,63 0 1,1 -0.1,0"/></defs>
              <text fill="rgba(255,255,255,.8)" font-size="11.5" letter-spacing="3.2" font-family="Outfit" font-weight="800">
                <textPath href="#circPath">• STREAM IN 4K • 3D IMMERSION </textPath>
              </text>
            </svg>
            <div class="core">
              <button data-action="go-watch" data-id="${item.id}" aria-label="Watch now">${ic.play}</button>
            </div>
          </div>

          <div class="trend-card-3d">
            <div style="width:36px;height:36px;border-radius:50%;background:var(--red);display:flex;align-items:center;justify-content:center;box-shadow:0 4px 14px var(--red-glow);">${ic.star}</div>
            <div>
              <div style="font-size:16px;font-weight:800;line-height:1.1;">${item.rating || '8.8'}<span style="font-size:11px;color:var(--text-faint);font-weight:600;"> /10</span></div>
              <div style="font-size:10px;color:var(--text-faint);text-transform:uppercase;letter-spacing:0.05em;font-weight:700;">Critic Score</div>
            </div>
          </div>

          <div class="cast-strip-3d">
            <div class="lbl">Starring</div>
            <div class="cast-avatars">
              ${(item.cast || ['Aarav', 'Nina', 'Marcus']).slice(0, 3).map((c, i) => `
                <div class="av" style="background:${['#e50914', '#7a3d9c', '#15607f'][i % 3]}">
                  ${(typeof c === 'string' ? c : c.name || 'A').replace('Voice: ', '').replace('Narrated by ', '')[0]}
                </div>`).join('')}
            </div>
          </div>
        </div>
      </div>

      <div class="hero-dots">
        ${getHeroPool().map((h, i) => `<span class="hdot ${i === state.heroIndex ? 'active' : ''}" data-action="hero-goto" data-idx="${i}"></span>`).join('')}
      </div>

      <div class="hero-arrows">
        <button class="icon-btn" data-action="hero-prev" aria-label="Previous">${ic.chevL}</button>
        <button class="icon-btn" data-action="hero-next" aria-label="Next">${ic.chevR}</button>
      </div>
    </div>`;
  }

  function getHeroPool() {
    const list = TITLES.filter(t => t.isTrending || t.backdrop).slice(0, 5);
    return list.length ? list : INITIAL_TITLES.slice(0, 4);
  }

  function previewStripHTML(item) {
    if (!item) return '';
    const bg1 = item.backdrop || primaryGenreColor(item).img;
    const bg2 = item.poster || primaryGenreColor(item).img;
    return `
    <div class="preview-strip">
      <div class="container preview-cards">
        <div class="preview-card" data-action="open-trailer" data-id="${item.id}" data-kind="trailer" style="background-image:url('${bg1}');">
          <div class="pv-overlay">
            <div class="pv-play">${ic.play}</div>
            <div>
              <div class="pv-label">Official 4K Trailer</div>
              <div class="pv-title">${esc(item.title)}</div>
              <div class="pv-dur">${item.trailerLen || '2:14'}</div>
            </div>
          </div>
        </div>
        <div class="preview-card" data-action="open-trailer" data-id="${item.id}" data-kind="teaser" style="background-image:url('${bg2}');">
          <div class="pv-overlay">
            <div class="pv-play">${ic.play}</div>
            <div>
              <div class="pv-label">Cinematic Teaser</div>
              <div class="pv-title">${esc(item.title)}</div>
              <div class="pv-dur">${item.teaserLen || '0:45'}</div>
            </div>
          </div>
        </div>
      </div>
    </div>`;
  }

  function popularSectionHTML() {
    const activeCat = state.popularCat || 'hollywood';
    const items = TITLES.filter(t => t.category === activeCat);
    const tabs = CATS.filter(c => c.key !== 'all');
    return `
    <section class="section reveal in-view">
      <div class="container">
        <div class="section-head">
          <div>
            <h2 class="disp section-title">Popular Worldwide</h2>
            <div class="section-sub">Top ranked films and web series across languages</div>
          </div>
        </div>
        <div class="tabbar hide-scroll">
          ${tabs.map(t => `<button class="tabbtn ${t.key === activeCat ? 'active' : ''}" data-action="popular-tab" data-cat="${t.key}">${t.label}</button>`).join('')}
        </div>
        <div class="rail-wrap">
          <button class="rail-arrow left" data-action="rail-scroll" data-target="popularRail" data-dir="-1">${ic.chevL}</button>
          <div class="rail hide-scroll" id="popularRail">
            ${items.length ? items.map(cardHTML).join('') : '<div style="color:var(--text-dim);padding:30px 0;">No titles in this category.</div>'}
          </div>
          <button class="rail-arrow right" data-action="rail-scroll" data-target="popularRail" data-dir="1">${ic.chevR}</button>
        </div>
      </div>
    </section>`;
  }

  function genreGridHTML() {
    return `
    <section class="section reveal in-view">
      <div class="container">
        <div class="section-head">
          <div>
            <h2 class="disp section-title">Explore by Genre</h2>
            <div class="section-sub">Dive into curated universes by atmosphere &amp; style</div>
          </div>
        </div>
        <div class="genre-grid">
          ${GENRES.map(g => `
            <div class="genre-tile" data-action="genre-click" data-genre="${g.name}">
              <div class="gbg grain" style="background-image:url('${g.img}');"></div>
              <div class="gname">${g.name}</div>
            </div>`).join('')}
        </div>
      </div>
    </section>`;
  }

  function renderHome() {
    const heroList = getHeroPool();
    const currentHero = heroList[state.heroIndex] || heroList[0];
    const trending = TITLES.filter(t => t.isTrending);
    const recent = TITLES.filter(t => t.isNew);
    const recommended = TITLES.slice().reverse().slice(0, 10);
    const continueList = state.history.length ? state.history : [
      { id: currentHero.id, pct: 70, label: 'Resume' }
    ];

    return `
      <section id="hero">
        ${heroList.map((item, i) => heroSlideHTML(item, i)).join('')}
      </section>
      ${previewStripHTML(currentHero)}
      ${railSection({ title: 'Trending Cinema', sub: 'The most watched blockbusters this week', items: trending, seeAllAction: 'see-trending', railId: 'railTrending' })}
      ${popularSectionHTML()}
      ${railSection({ title: 'New Releases', sub: 'Fresh 4K additions on HPSTREAM', items: recent, seeAllAction: 'see-recent', railId: 'railRecent' })}
      ${continueList.length ? railSection({ title: 'Continue Watching', sub: 'Jump straight back into your timeline', items: continueList, seeAllAction: null, railId: 'railContinue', cardFn: progressCardHTML }) : ''}
      ${railSection({ title: 'Recommended For You', sub: 'Hand-picked based on your cinematic taste', items: recommended, seeAllAction: 'see-recommended', railId: 'railRecommended' })}
      ${genreGridHTML()}
    `;
  }

  /* ============================================================
     DETAILS PAGE (MOVIE & SERIES)
     ============================================================ */
  function renderDetail(id, seasonNum) {
    const item = byId[id] || INITIAL_TITLES[0];
    if (!item) return renderHome();
    const g = primaryGenreColor(item);
    const inWL = state.watchlist.has(item.id);
    const bgUrl = item.backdrop || item.poster || g.img;
    const posterUrl = item.poster || g.img;
    const isSeries = item.type === 'series';

    let season = null;
    if (isSeries && item.seasons && item.seasons.length) {
      season = item.seasons.find(s => s.season === seasonNum) || item.seasons[0];
    }

    const castText = (item.cast || ['Adrian Cole', 'Nina Marsh']).map(c => typeof c === 'string' ? c : c.name).join(', ');
    const directorText = item.director || 'Studio Production';
    const similar = TITLES.filter(t => t.id !== item.id && t.genres.some(g2 => item.genres.includes(g2))).slice(0, 8);

    return `
    <section class="detail-hero grain" style="background-image:url('${bgUrl}');">
      <div class="container detail-content">
        <div class="detail-poster">
          <img src="${posterUrl}" alt="${esc(item.title)}" style="width:100%;height:100%;object-fit:cover;">
        </div>
        <div class="detail-info">
          <div class="hero-badge-row">
            <span class="pill pill-red">${isSeries ? 'Series' : 'Movie'}</span>
            ${item.isTrending ? '<span class="pill pill-line">Trending</span>' : ''}
            ${item.isNew ? '<span class="pill pill-line">New</span>' : ''}
            <span class="pill pill-line">4K Ultra HD</span>
          </div>
          <h1 class="disp detail-title">${esc(item.title)}</h1>
          <div class="detail-meta-row">
            <span class="rating">${ic.star} ${item.rating || '8.6'}</span>
            <span class="dot-sep"></span>
            <span>${item.year || '2026'}</span>
            <span class="dot-sep"></span>
            ${item.duration ? `<span>${item.duration}</span><span class="dot-sep"></span>` : (isSeries && item.seasons ? `<span>${item.seasons.length} Season${item.seasons.length > 1 ? 's' : ''}</span><span class="dot-sep"></span>` : '')}
            <span>${item.age || 'U/A 16+'}</span>
            <span class="dot-sep"></span>
            <span>${(item.quality || ['4K', 'HD']).join(' · ')}</span>
            <span class="dot-sep"></span>
            <span>${(item.languages || ['English']).join(' / ')}</span>
          </div>
          <div class="tag-row">
            ${(item.genres || ['Action']).map(g2 => `<span class="tag">${g2}</span>`).join('')}
          </div>
          <p class="detail-desc">${esc(item.description)}</p>
          <div class="hero-btn-row">
            <button class="btn btn-primary" data-action="go-watch" data-id="${item.id}">
              ${ic.play} Stream Now
            </button>
            <button class="btn btn-ghost" data-action="open-trailer" data-id="${item.id}" data-kind="trailer">
              ${ic.play} Trailer
            </button>
            <button class="btn btn-outline" data-action="toggle-watchlist" data-id="${item.id}">
              ${inWL ? ic.check + ' In Watchlist' : ic.plus + ' Watchlist'}
            </button>
          </div>
        </div>
      </div>
    </section>

    ${isSeries && season ? `
    <div class="container" style="margin-top:30px;">
      <h3 class="disp" style="font-size:28px;letter-spacing:0.02em;">Episodes</h3>
      <div class="season-selector">
        ${item.seasons.map(s => `<button class="tabbtn ${s.season === season.season ? 'active' : ''}" data-action="series-season" data-id="${item.id}" data-season="${s.season}">Season ${s.season}</button>`).join('')}
      </div>
      <div>
        ${(season.episodes || []).map(e => `
          <div class="episode-row" data-action="go-watch-episode" data-id="${item.id}" data-season="${season.season}" data-ep="${e.ep}">
            <div class="ep-num">${String(e.ep).padStart(2, '0')}</div>
            <div class="ep-thumb">
              <img src="${e.still || posterUrl}" alt="${esc(e.title)}" style="width:100%;height:100%;object-fit:cover;">
              <div class="ep-play"><div class="card-play" style="transform:none;">${ic.play}</div></div>
            </div>
            <div class="ep-body">
              <div class="ep-title-row">
                <h4>${esc(e.title)}</h4>
                <span class="ep-dur">${e.dur || '45m'}</span>
              </div>
              <p class="ep-desc">${esc(e.desc)}</p>
            </div>
          </div>`).join('')}
      </div>
    </div>` : ''}

    <div class="container">
      <div class="info-table reveal in-view">
        <div class="info-row"><div class="k">Cast</div><div class="v">${castText || 'Ensemble Cast'}</div></div>
        <div class="info-row"><div class="k">Director</div><div class="v">${directorText}</div></div>
        <div class="info-row"><div class="k">Genres</div><div class="v">${(item.genres || []).join(', ')}</div></div>
        <div class="info-row"><div class="k">Audio Languages</div><div class="v">${(item.languages || []).join(', ')}</div></div>
        <div class="info-row"><div class="k">Release Date</div><div class="v">${item.year || '2026'}</div></div>
        <div class="info-row"><div class="k">Streaming Resolution</div><div class="v">4K Ultra HD, Dolby Vision, Dolby Atmos 5.1</div></div>
      </div>
    </div>

    ${railSection({ title: 'More Like This', sub: 'Titles you might also enjoy', items: similar.length ? similar : TITLES.slice(0, 6), seeAllAction: null, railId: 'railSimilar' })}
    `;
  }

  /* ============================================================
     WATCH PAGE (PROFESSIONAL CINEMA PLAYER ENGINE)
     ============================================================ */

  // Professional stream server configs
  const STREAM_SERVERS = {
    vidsrc: {
      name: 'VidSrc',
      icon: '⚡',
      badge: 'HD',
      getUrl: (item, season, ep) => {
        const tmdbId = item.tmdbId;
        if (!tmdbId) return null;
        if (item.type === 'series') {
          return `https://vidsrc.to/embed/tv/${tmdbId}/${season}/${ep}`;
        }
        return `https://vidsrc.to/embed/movie/${tmdbId}`;
      }
    },
    embed: {
      name: 'EmbedSu',
      icon: '🎬',
      badge: '4K',
      getUrl: (item, season, ep) => {
        const tmdbId = item.tmdbId;
        if (!tmdbId) return null;
        if (item.type === 'series') {
          return `https://embed.su/embed/tv/${tmdbId}/${season}/${ep}`;
        }
        return `https://embed.su/embed/movie/${tmdbId}`;
      }
    },
    autoembed: {
      name: 'AutoEmbed',
      icon: '🌐',
      badge: 'HD',
      getUrl: (item, season, ep) => {
        const tmdbId = item.tmdbId;
        if (!tmdbId) return null;
        if (item.type === 'series') {
          return `https://autoembed.co/tv/tmdb/${tmdbId}-${season}-${ep}`;
        }
        return `https://autoembed.co/movie/tmdb/${tmdbId}`;
      }
    },
    multiembed: {
      name: 'MultiEmbed',
      icon: '🚀',
      badge: 'FHD',
      getUrl: (item, season, ep) => {
        const tmdbId = item.tmdbId;
        if (!tmdbId) return null;
        if (item.type === 'series') {
          return `https://multiembed.mov/?video_id=${tmdbId}&tmdb=1&s=${season}&e=${ep}`;
        }
        return `https://multiembed.mov/?video_id=${tmdbId}&tmdb=1`;
      }
    },
    '2embed': {
      name: '2Embed',
      icon: '💎',
      badge: '4K',
      getUrl: (item, season, ep) => {
        const tmdbId = item.tmdbId;
        if (!tmdbId) return null;
        if (item.type === 'series') {
          return `https://www.2embed.cc/embedtv/${tmdbId}&s=${season}&e=${ep}`;
        }
        return `https://www.2embed.cc/embed/${tmdbId}`;
      }
    }
  };

  // Current player state
  let playerState = {
    currentServer: 'vidsrc',
    currentLang: null,
    currentItem: null,
    currentSeason: null,
    currentEp: null
  };

  function initProPlayer(item, season, ep) {
    playerState.currentItem = item;
    playerState.currentSeason = season;
    playerState.currentEp = ep;
    playerState.currentLang = (item.languages || ['English'])[0];
    loadPlayerServer(playerState.currentServer);
  }

  function loadPlayerServer(serverKey) {
    const player = document.getElementById('videoContainer');
    const loading = document.getElementById('playerLoading');
    const statusDot = document.getElementById('serverStatusDot');
    const statusText = document.getElementById('serverStatusText');
    const statusPing = document.getElementById('serverStatusPing');
    if (!player) return;

    const item = playerState.currentItem;
    const server = STREAM_SERVERS[serverKey];
    if (!server) return;

    // Set loading state
    if (loading) loading.style.display = 'flex';
    if (statusDot) statusDot.className = 'server-status-dot loading';
    if (statusText) statusText.textContent = `${server.name} connecting...`;
    if (statusPing) statusPing.textContent = '';

    // Try Supabase stream URL first, fallback to direct embed
    const tryLoad = async () => {
      let url = null;
      const startTime = Date.now();

      // First try: our Supabase backend
      try {
        url = await getStreamUrl(item, playerState.currentSeason, playerState.currentEp);
      } catch {}

      // Second try: direct server URL
      if (!url && server.getUrl) {
        url = server.getUrl(item, playerState.currentSeason, playerState.currentEp);
      }

      // Final fallback: trailer
      if (!url) {
        url = item.youtubeTrailer || 'https://www.youtube.com/embed/dQw4w9WgXcQ?autoplay=1';
      }

      const ping = Date.now() - startTime;

      // Construct iframe safely with complete fullscreen permissions
      const iframe = document.createElement('iframe');
      iframe.src = url;
      iframe.title = item.title;
      iframe.setAttribute('allow', 'accelerometer *; autoplay *; clipboard-write *; encrypted-media *; gyroscope *; picture-in-picture *; web-share *; fullscreen *');
      iframe.setAttribute('allowfullscreen', 'true');
      iframe.setAttribute('webkitallowfullscreen', 'true');
      iframe.setAttribute('mozallowfullscreen', 'true');
      iframe.allow = "accelerometer *; autoplay *; clipboard-write *; encrypted-media *; gyroscope *; picture-in-picture *; web-share *; fullscreen *";
      iframe.allowFullscreen = true;
      iframe.referrerPolicy = "no-referrer";
      iframe.style.width = "100%";
      iframe.style.height = "100%";
      iframe.style.border = "0";
      iframe.style.display = "block";

      // Clear previous content but keep loading if still loading
      player.querySelectorAll('iframe').forEach(el => el.remove());
      player.appendChild(iframe);

      if (iframe) {
        iframe.onload = () => {
          player.classList.add('loaded');
          if (loading) loading.style.display = 'none';
          if (statusDot) statusDot.className = 'server-status-dot online';
          if (statusText) statusText.textContent = `${server.name} — Stream active, ${esc(item.title)}`;
          if (statusPing) statusPing.textContent = `${Date.now() - startTime}ms`;
        };
        // Fallback timeout
        setTimeout(() => {
          if (!player.classList.contains('loaded')) {
            player.classList.add('loaded');
            if (loading) loading.style.display = 'none';
            if (statusDot) statusDot.className = 'server-status-dot online';
            if (statusText) statusText.textContent = `${server.name} — Stream active, ${esc(item.title)}`;
            if (statusPing) statusPing.textContent = `${Date.now() - startTime}ms`;
          }
        }, 10000);
      } else {
        if (loading) loading.style.display = 'none';
        if (statusDot) statusDot.className = 'server-status-dot online';
        if (statusText) statusText.textContent = `${server.name} — Stream active, ${esc(item.title)}`;
        if (statusPing) statusPing.textContent = `${ping}ms`;
      }
    };

    tryLoad().catch(() => {
      if (loading) loading.style.display = 'none';
      if (statusDot) statusDot.className = 'server-status-dot offline';
      if (statusText) statusText.textContent = `${server.name} unavailable — dusra server try karein`;
      player.innerHTML = `
        <div class="player-error">
          <div class="player-error-icon">⚠️</div>
          <div class="player-error-title">Server Unavailable</div>
          <div class="player-error-sub">Koi doosra server select karein ya baad mein try karein.</div>
        </div>
      `;
    });
  }

  function renderPlayerTopCenterOverlay(item, season, epObj, isSeries) {
    return '';
  }

  function renderPlayerBottomOverlay(item, season, epObj, isSeries) {
    const nextEpBtn = (isSeries && epObj) ? `<button class="player-ctrl-btn" data-action="go-watch-episode" data-id="${item.id}" data-season="${season.season}" data-ep="${epObj.ep + 1}" title="Next Episode"><svg viewBox="0 0 24 24" fill="currentColor" width="18" height="18"><polygon points="5 4 15 12 5 20 5 4"/><line x1="19" y1="5" x2="19" y2="19" stroke="currentColor" stroke-width="2.5"/></svg></button>` : '';

    return `
      <div class="player-bottom-bar" id="playerBottomBar">
        <div class="player-bottom-controls">
          <div class="player-bottom-left">
            ${nextEpBtn}
          </div>

          <div class="player-bottom-right">
            <button class="player-ctrl-btn" data-action="player-toggle-theater" title="Theater Mode (T)"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" width="18" height="18"><rect x="2" y="4" width="20" height="16" rx="2"/><line x1="2" y1="8" x2="22" y2="8"/></svg></button>
            <button class="player-ctrl-btn" data-action="player-toggle-fullscreen" title="Fullscreen (F)"><svg class="fs-open" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" width="18" height="18"><path d="M8 3H5a2 2 0 0 0-2 2v3m18 0V5a2 2 0 0 0-2-2h-3m0 18h3a2 2 0 0 0 2-2v-3M3 16v3a2 2 0 0 0 2 2h3"/></svg><svg class="fs-close" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" width="18" height="18" style="display:none;"><path d="M8 3v3a2 2 0 0 1-2 2H3m18 0h-3a2 2 0 0 1-2-2V3m0 18v-3a2 2 0 0 1 2-2h3M3 16h3a2 2 0 0 1 2 2v3"/></svg></button>
          </div>
        </div>
      </div>`;
  }

  function renderWatch(id, params) 
  {
    const item = byId[id] || INITIAL_TITLES[0];
    if (!item) return renderHome();
    const isSeries = item.type === 'series';
    let season = null;
    let epObj = null;

    if (isSeries && item.seasons && item.seasons.length) {
      const sNum = params.season || item.seasons[0].season;
      season = item.seasons.find(s => s.season === sNum) || item.seasons[0];
      epObj = season.episodes.find(e => e.ep === (params.ep || 1)) || season.episodes[0];
    }

    const inWL = state.watchlist.has(item.id);
    const recPool = TITLES.filter(t => t.id !== item.id).slice(0, 8);

    // Save to history
    saveHistoryItem(item.id, state.playerPct, isSeries && season && epObj ? `S${season.season} · E${epObj.ep}` : 'In Progress');

    const html = `
    <div class="watch-page">
      <div class="hp-video-player" id="hpVideoPlayer">
        <div class="video-container" id="videoContainer">
          <div class="player-loading" id="playerLoading">
            <div class="player-loading-inner">
               <div class="player-spinner"></div>
               <div class="player-loading-title">${esc(item.title)}</div>
               <div class="player-loading-sub">Preparing stream...</div>
            </div>
          </div>
        </div>

        <!-- Professional Interactive Overlay Controls -->
        <div class="player-controls-overlay" id="playerControlsOverlay">
          ${renderPlayerBottomOverlay(item, season, epObj, isSeries)}
        </div>
      </div>

      <div class="container watch-below">
        <div class="watch-info">
          <h1>${esc(item.title)}${isSeries && season && epObj ? ` <span style="color:var(--text-faint);">— S${season.season} · E${String(epObj.ep).padStart(2, '0')}: ${esc(epObj.title)}</span>` : ''}</h1>
          <div class="wmeta">
            <span class="rating">${ic.star} ${item.rating || '8.8'}</span>
            <span class="dot-sep"></span>
            <span>${item.year || '2026'}</span>
            <span class="dot-sep"></span>
            <span>${item.age || 'U/A 16+'}</span>
            <span class="dot-sep"></span>
            <span>${isSeries && epObj ? epObj.dur : (item.duration || '2h 15m')}</span>
            <span class="dot-sep"></span>
            <span style="color:var(--cyan);font-weight:700;">4K STREAM ACTIVE</span>
          </div>
          <p>${esc(isSeries && epObj ? epObj.desc : item.description)}</p>

          ${isSeries && season ? `
          <div class="ep-nav-row">
            <button class="btn btn-outline btn-sm" data-action="open-detail" data-id="${item.id}">
              View All Episodes
            </button>
          </div>` : ''}
        </div>

        <div class="watch-actions">
          <button class="btn btn-primary btn-sm" data-action="player-toggle-fullscreen" title="Fullscreen (F)">
            <svg class="fs-open" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" width="16" height="16" style="vertical-align:middle;margin-right:6px;"><path d="M8 3H5a2 2 0 0 0-2 2v3m18 0V5a2 2 0 0 0-2-2h-3m0 18h3a2 2 0 0 0 2-2v-3M3 16v3a2 2 0 0 0 2 2h3"/></svg>
            <svg class="fs-close" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" width="16" height="16" style="display:none;vertical-align:middle;margin-right:6px;"><path d="M8 3v3a2 2 0 0 1-2 2H3m18 0h-3a2 2 0 0 1-2-2V3m0 18v-3a2 2 0 0 1 2-2h3M3 16h3a2 2 0 0 1 2 2v3"/></svg>
            Fullscreen
          </button>
          <button class="btn btn-outline btn-sm" data-action="player-toggle-theater" title="Theater Mode (T)">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" width="16" height="16" style="vertical-align:middle;margin-right:6px;"><rect x="2" y="4" width="20" height="16" rx="2"/><line x1="2" y1="8" x2="22" y2="8"/></svg>
            Theater
          </button>
          <button class="btn btn-outline btn-sm" data-action="toggle-watchlist" data-id="${item.id}">
            ${inWL ? ic.check + ' In Watchlist' : ic.plus + ' Watchlist'}
          </button>
          <button class="btn btn-outline btn-sm" data-action="share" data-id="${item.id}">
            ${ic.share} Share
          </button>
        </div>
      </div>

      ${isSeries && season ? `
      <div class="container" style="margin-bottom:40px;">
        <h3 class="disp" style="font-size:24px;margin-bottom:16px;">Next in Season ${season.season}</h3>
        ${(season.episodes || []).map(e => `
          <div class="episode-row ${epObj && e.ep === epObj.ep ? 'active' : ''}" data-action="go-watch-episode" data-id="${item.id}" data-season="${season.season}" data-ep="${e.ep}">
            <div class="ep-num">${String(e.ep).padStart(2, '0')}</div>
            <div class="ep-thumb">
              <img src="${e.still || item.poster || primaryGenreColor(item).img}" alt="${esc(e.title)}" style="width:100%;height:100%;object-fit:cover;">
              <div class="ep-play"><div class="card-play" style="transform:none;">${ic.play}</div></div>
            </div>
            <div class="ep-body">
              <div class="ep-title-row">
                <h4>${esc(e.title)}</h4>
                <span class="ep-dur">${e.dur || '45m'}</span>
              </div>
              <p class="ep-desc">${esc(e.desc)}</p>
            </div>
          </div>`).join('')}
      </div>` : ''}

      ${railSection({ title: 'Up Next in 4K', sub: 'Recommended streams for your watch queue', items: recPool, seeAllAction: null, railId: 'railUpNext' })}
    </div>`;
    
    // After render: initialize professional player
    setTimeout(() => {
      initProPlayer(item, isSeries ? (season ? season.season : 1) : null, isSeries ? (epObj ? epObj.ep : 1) : null);
    }, 0);

    return html;
  }

  /* ============================================================
     SEARCH & BROWSE PAGE
     ============================================================ */
  function renderBrowse(p) {
    p = p || {};
    const query = (p.q || '').toLowerCase().trim();
    let items = TITLES.slice();

    if (query) {
      items = items.filter(t =>
        t.title.toLowerCase().includes(query) ||
        (t.genres || []).some(g => g.toLowerCase().includes(query)) ||
        (t.cast || []).some(c => (typeof c === 'string' ? c : c.name || '').toLowerCase().includes(query))
      );
    }
    if (p.genre) {
      items = items.filter(t => (t.genres || []).includes(p.genre));
    }
    if (p.category && p.category !== 'all') {
      items = items.filter(t => t.category === p.category);
    }

    const f = state.activeFilters || {};
    if (f.type) items = items.filter(t => t.type === f.type);
    if (f.language) items = items.filter(t => (t.languages || []).includes(f.language));
    if (f.genre) items = items.filter(t => (t.genres || []).includes(f.genre));
    if (f.year) items = items.filter(t => String(t.year) === f.year);
    if (f.rating) items = items.filter(t => Number(t.rating) >= parseFloat(f.rating));

    const sort = f.sort || 'trending';
    if (sort === 'latest') items.sort((a, b) => (b.year || 0) - (a.year || 0));
    else if (sort === 'rating') items.sort((a, b) => (Number(b.rating) || 0) - (Number(a.rating) || 0));
    else if (sort === 'az') items.sort((a, b) => a.title.localeCompare(b.title));
    else items.sort((a, b) => (b.isTrending ? 1 : 0) - (a.isTrending ? 1 : 0));

    const heading = p.genre ? `${p.genre} Cinema` : (p.category && p.category !== 'all' ? (CATS.find(c => c.key === p.category)?.label || 'Browse') : (p.title || 'Explore Library'));

    return `
    <div class="browse-page container">
      <h1 class="disp section-title reveal in-view" style="margin-bottom:20px;">${heading}</h1>

      <div class="search-bar-wrap reveal in-view">
        <span class="search-icon-l">${ic.search}</span>
        <input class="search-input" id="searchInput" placeholder="Search movies, anime, series, directors, actors…" value="${p.q ? esc(p.q) : ''}" autocomplete="off">
        ${p.q ? `<button class="search-clear" data-action="clear-search" aria-label="Clear">${ic.close}</button>` : ''}
        <div id="suggestBox"></div>
      </div>

      <div class="filter-bar reveal in-view">
        <div class="result-count">${items.length} title${items.length === 1 ? '' : 's'} available in 4K &amp; HD</div>
        <button class="btn btn-outline btn-sm filter-toggle" data-action="toggle-filters">
          ${ic.filter} Filter &amp; Sort
        </button>
      </div>

      <div class="filters-panel" id="filtersPanel">
        <div class="f-group">
          <label>Type</label>
          <select data-fkey="type">
            <option value="">All Types</option>
            <option value="movie" ${f.type === 'movie' ? 'selected' : ''}>Movies</option>
            <option value="series" ${f.type === 'series' ? 'selected' : ''}>Series / Anime</option>
          </select>
        </div>
        <div class="f-group">
          <label>Language</label>
          <select data-fkey="language">
            <option value="">All Languages</option>
            ${['English', 'Hindi', 'Japanese', 'Korean'].map(l => `<option value="${l}" ${f.language === l ? 'selected' : ''}>${l}</option>`).join('')}
          </select>
        </div>
        <div class="f-group">
          <label>Genre</label>
          <select data-fkey="genre">
            <option value="">All Genres</option>
            ${GENRES.map(g => `<option value="${g.name}" ${f.genre === g.name ? 'selected' : ''}>${g.name}</option>`).join('')}
          </select>
        </div>
        <div class="f-group">
          <label>Year</label>
          <select data-fkey="year">
            <option value="">All Years</option>
            ${['2026', '2025', '2024', '2023'].map(y => `<option value="${y}" ${f.year === y ? 'selected' : ''}>${y}</option>`).join('')}
          </select>
        </div>
        <div class="f-group">
          <label>Min Rating</label>
          <select data-fkey="rating">
            <option value="">Any Rating</option>
            <option value="8.5" ${f.rating === '8.5' ? 'selected' : ''}>8.5+ Top Rated</option>
            <option value="8.0" ${f.rating === '8.0' ? 'selected' : ''}>8.0+</option>
            <option value="7.0" ${f.rating === '7.0' ? 'selected' : ''}>7.0+</option>
          </select>
        </div>
        <div class="f-group">
          <label>Sort Order</label>
          <select data-fkey="sort">
            <option value="trending" ${sort === 'trending' ? 'selected' : ''}>Trending Now</option>
            <option value="latest" ${sort === 'latest' ? 'selected' : ''}>Latest Release</option>
            <option value="rating" ${sort === 'rating' ? 'selected' : ''}>Highest Rating</option>
            <option value="az" ${sort === 'az' ? 'selected' : ''}>Alphabetical (A–Z)</option>
          </select>
        </div>
      </div>

      ${items.length ? `
      <div class="results-grid reveal in-view">
        ${items.map(cardHTML).join('')}
      </div>` : `
      <div class="empty-state reveal in-view">
        <div class="disp">No Cinema Results Found</div>
        <p>Try searching for a different keyword, actor, or clearing filters.</p>
        <button class="btn btn-primary" data-action="clear-search">Reset Library</button>
      </div>`}
    </div>`;
  }

  /* ============================================================
     WATCHLIST & PROFILE PAGES
     ============================================================ */
  function renderWatchlist() {
    const items = Array.from(state.watchlist).map(id => byId[id]).filter(Boolean);
    return `
    <div class="simple-page container">
      <h1 class="disp page-title reveal in-view">My Cinema Watchlist</h1>
      <div class="page-sub reveal in-view">${items.length} title${items.length === 1 ? '' : 's'} saved to your personalized library</div>
      ${items.length ? `
      <div class="watchlist-grid reveal in-view">
        ${items.map(item => `
          <div class="card-3d-wrap" data-action="open-detail" data-id="${item.id}" style="position:relative;">
            <div class="card-3d">
              <div class="card-poster">
                <div class="poster-art grain" style="background-image:url('${item.poster || primaryGenreColor(item).img}');"></div>
                <button class="wl-remove" data-action="remove-watchlist" data-id="${item.id}" aria-label="Remove" title="Remove from Watchlist">${ic.close}</button>
                <div class="card-hover-overlay"><div class="card-play">${ic.play}</div></div>
                <div class="card-glare"></div>
              </div>
              <div class="card-meta">
                <div class="card-title">${esc(item.title)}</div>
                <div class="card-sub">${fmtRating(item)}<span class="dot-sep"></span>${item.year || '2026'}</div>
              </div>
            </div>
          </div>`).join('')}
      </div>` : `
      <div class="empty-state reveal in-view">
        <div class="disp">Your Watchlist is Empty</div>
        <p>Save blockbusters, anime, and series to stream whenever you are ready.</p>
        <button class="btn btn-primary" data-action="go-home-browse">Discover Movies</button>
      </div>`}
    </div>`;
  }

  function renderProfile() {
    const historyItems = state.history.map(h => byId[h.id]).filter(Boolean);
    const favGenres = ['Action', 'Thriller', 'Sci-Fi', 'Drama', 'Anime'];

    return `
    <div class="simple-page container">
      <div class="profile-shell">
        <div class="profile-card reveal in-view">
          <div class="avatar-big">HP</div>
          <h3>Cinema Premier User</h3>
          <div class="email">user@hpstream.cinema</div>
          <div class="profile-menu">
            <a href="#" data-action="go-watchlist">${ic.heart} My Watchlist (${state.watchlist.size})</a>
            <a href="#" data-action="scroll-continue">${ic.play} Watch History</a>
            <a href="#" data-action="noop">${ic.gear} Playback Quality (4K Ultra HD)</a>
            <a href="#" data-action="logout" style="color:var(--red);">Sign Out</a>
          </div>
        </div>
        <div>
          <div class="profile-section reveal in-view" id="pv-continue">
            <h3>Resume Playback</h3>
            <div class="watchlist-grid">
              ${state.history.length ? state.history.map(progressCardHTML).join('') : '<p style="color:var(--text-dim);">No active watch history.</p>'}
            </div>
          </div>
          <div class="profile-section reveal in-view" id="pv-recent">
            <h3>Recently Viewed</h3>
            <div class="watchlist-grid">
              ${historyItems.length ? historyItems.map(cardHTML).join('') : '<p style="color:var(--text-dim);">No recent titles.</p>'}
            </div>
          </div>
          <div class="profile-section reveal in-view">
            <h3>Favorite Genres</h3>
            <div class="genre-chip-row">
              ${favGenres.map(g => `<span class="tag" data-action="genre-click" data-genre="${g}" style="cursor:pointer;">${g}</span>`).join('')}
            </div>
          </div>
        </div>
      </div>
    </div>`;
  }

  /* ============================================================
     FOOTER
     ============================================================ */
  function footerHTML() {
    return `
    <footer id="site-footer">
      <div class="container">
        <div class="footer-grid">
          <div class="footer-brand">
            <div class="logo">HPSTREAM<span class="dot">.</span></div>
            <p>The ultimate destination for cinematic films, anime series, and world-class storytelling in 4K resolution and 3D visual depth.</p>
            <div class="social-row">
              <a class="social-circ" href="#" title="Instagram">IG</a>
              <a class="social-circ" href="#" title="X (Twitter)">X</a>
              <a class="social-circ" href="#" title="YouTube">YT</a>
              <a class="social-circ" href="#" title="Discord">DC</a>
            </div>
          </div>
          <div class="footer-col">
            <h4>Browse</h4>
            <a href="#" data-action="nav-cat" data-cat="hollywood">Hollywood Cinema</a>
            <a href="#" data-action="nav-cat" data-cat="bollywood">Bollywood Premiere</a>
            <a href="#" data-action="nav-cat" data-cat="anime">Anime Series</a>
            <a href="#" data-action="nav-cat" data-cat="kdrama">K-Drama Universe</a>
            <a href="#" data-action="nav-cat" data-cat="webseries">Web Originals</a>
          </div>
          <div class="footer-col">
            <h4>Discover</h4>
            <a href="#" data-action="see-trending">Trending Top 10</a>
            <a href="#" data-action="see-recent">New Releases</a>
            <a href="#" data-action="see-recommended">Recommended</a>
            <a href="#" data-action="go-watchlist">My Watchlist</a>
          </div>
          <div class="footer-col">
            <h4>HPSTREAM 3D</h4>
            <a href="#" data-action="noop">4K Ultra HD Engine</a>
            <a href="#" data-action="noop">Dolby Atmos Audio</a>
            <a href="#" data-action="noop">Spatial Ambient Lighting</a>
            <a href="#" data-action="noop">Terms &amp; Privacy</a>
          </div>
        </div>
        <div class="footer-bottom">
          <span>&copy; 2026 HPSTREAM Cinema. All rights reserved.</span>
          <span>Crafted for True Cinephiles.</span>
        </div>
      </div>
    </footer>`;
  }

  /* ============================================================
     MASTER RENDER PIPELINE
     ============================================================ */
  function render() {
    const app = document.getElementById('app');
    if (!app) return;

    let body = '';
    switch (state.view) {
      case 'home': body = renderHome(); break;
      case 'movie': body = renderDetail(state.params.id); break;
      case 'series': body = renderDetail(state.params.id, state.params.season); break;
      case 'watch': body = renderWatch(state.params.id, state.params); break;
      case 'browse': body = renderBrowse(state.params); break;
      case 'watchlist': body = renderWatchlist(); break;
      case 'profile': body = renderProfile(); break;
      default: body = renderHome();
    }

    app.innerHTML = `
      <header id="site-header" class="${window.scrollY > 10 ? 'scrolled' : ''}">
        ${headerHTML()}
      </header>
      ${body}
      ${footerHTML()}
    `;

    const mobilePanel = document.querySelector('#mobile-nav .panel');
    if (mobilePanel) mobilePanel.innerHTML = mobileNavHTML();

    attachPostRenderBehaviors();
  }

  function navigate(view, params = {}) {
    state.view = view;
    state.params = params;
    window.scrollTo({ top: 0, behavior: 'auto' });
    render();
  }

  /* ============================================================
     3D INTERACTIVE TILT ENGINE (PERFECT GPU PERFORMANCE)
     ============================================================ */
  function init3DTilt() {
    const cards = document.querySelectorAll('.card-3d-wrap');
    cards.forEach(wrap => {
      const card = wrap.querySelector('.card-3d');
      if (!card || wrap.__tiltBound) return;
      wrap.__tiltBound = true;

      wrap.addEventListener('mousemove', (e) => {
        const rect = wrap.getBoundingClientRect();
        const x = (e.clientX - rect.left) / rect.width - 0.5;
        const y = (e.clientY - rect.top) / rect.height - 0.5;
        const rotX = -y * 18;
        const rotY = x * 18;

        card.style.transform = `perspective(1000px) rotateX(${rotX.toFixed(2)}deg) rotateY(${rotY.toFixed(2)}deg) scale3d(1.03, 1.03, 1.03)`;
        const glare = card.querySelector('.card-glare');
        if (glare) {
          glare.style.setProperty('--gx', `${((x + 0.5) * 100).toFixed(1)}%`);
          glare.style.setProperty('--gy', `${((y + 0.5) * 100).toFixed(1)}%`);
        }
      }, { passive: true });

      wrap.addEventListener('mouseleave', () => {
        card.style.transform = `perspective(1000px) rotateX(0deg) rotateY(0deg) scale3d(1, 1, 1)`;
      }, { passive: true });
    });
  }

  function setupScrollReveal() {
    const els = document.querySelectorAll('.reveal:not(.in-view)');
    if (!('IntersectionObserver' in window)) {
      els.forEach(el => el.classList.add('in-view'));
      return;
    }
    const io = new IntersectionObserver((entries) => {
      entries.forEach(en => {
        if (en.isIntersecting) {
          en.target.classList.add('in-view');
          io.unobserve(en.target);
        }
      });
    }, { threshold: 0.08 });
    els.forEach(el => io.observe(el));
  }

  function attachPostRenderBehaviors() {
    setupScrollReveal();
    init3DTilt();
  }

  /* ============================================================
     HERO ROTATION TIMER
     ============================================================ */
  function resetHeroTimer() {
    clearInterval(state.heroTimer);
    state.heroTimer = setInterval(() => {
      if (state.view === 'home') {
        const pool = getHeroPool();
        state.heroIndex = (state.heroIndex + 1) % pool.length;
        render();
      }
    }, 7500);
  }

  /* ============================================================
     MODALS & POPUPS
     ============================================================ */
  function openVideoModal(item, kind) {
    if (!item) return;
    const g = primaryGenreColor(item);
    const label = kind === 'teaser' ? 'Teaser Preview' : 'Official Trailer';
    const trailerUrl = item.youtubeTrailer || 'https://www.youtube.com/embed/dQw4w9WgXcQ?autoplay=1';

    const html = `
    <div class="modal-scrim video-modal open" id="modalScrim">
      <div class="modal-box">
        <div class="vm-player">
          <iframe
            src="${trailerUrl}"
            title="${esc(item.title)} ${label}"
            style="width:100%;height:100%;border:none;"
            allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share"
            allowfullscreen>
          </iframe>
          <button class="icon-btn vm-close" data-action="close-modal" aria-label="Close">${ic.close}</button>
        </div>
      </div>
    </div>`;

    document.getElementById('modal-root').innerHTML = html;
    document.getElementById('modalScrim').addEventListener('click', (e) => {
      if (e.target.id === 'modalScrim') closeModal();
    });
  }

  function openAuthModal(mode) {
    let inner = '';
    if (mode === 'login') {
      inner = `
        <h2>Welcome to HPSTREAM</h2>
        <div class="modal-sub">Sign in to stream in 4K, manage your watchlist, and save watch history.</div>
        <div class="form-field"><label>Email</label><input type="email" placeholder="cinema@hpstream.com"></div>
        <div class="form-field"><label>Password</label><input type="password" placeholder="••••••••"></div>
        <button class="btn btn-primary modal-full-btn" data-action="demo-login">Sign In</button>
        <button class="btn btn-ghost modal-full-btn" data-action="demo-login">Continue as Guest (Instant Access)</button>
        <div class="modal-switch">New to HPSTREAM? <a href="#" data-action="auth-switch" data-mode="register">Create 4K account</a></div>`;
    } else {
      inner = `
        <h2>Create VIP Account</h2>
        <div class="modal-sub">Join HPSTREAM for unlimited streaming across all devices.</div>
        <div class="form-field"><label>Full Name</label><input type="text" placeholder="Aria Sharma"></div>
        <div class="form-field"><label>Email</label><input type="email" placeholder="cinema@hpstream.com"></div>
        <div class="form-field"><label>Password</label><input type="password" placeholder="••••••••"></div>
        <button class="btn btn-primary modal-full-btn" data-action="demo-login">Create 4K Account</button>
        <div class="modal-switch">Already have an account? <a href="#" data-action="auth-switch" data-mode="login">Sign In</a></div>`;
    }

    const html = `
    <div class="modal-scrim open" id="modalScrim">
      <div class="modal-box">
        <button class="icon-btn modal-close" data-action="close-modal" aria-label="Close">${ic.close}</button>
        ${inner}
      </div>
    </div>`;

    document.getElementById('modal-root').innerHTML = html;
    document.getElementById('modalScrim').addEventListener('click', (e) => {
      if (e.target.id === 'modalScrim') closeModal();
    });
  }

  function closeModal() {
    document.getElementById('modal-root').innerHTML = '';
  }

  function showToast(msg) {
    const root = document.getElementById('toast-root');
    if (!root) return;
    const t = document.createElement('div');
    t.className = 'toast';
    t.innerHTML = `${ic.check} ${esc(msg)}`;
    root.appendChild(t);
    setTimeout(() => {
      t.style.opacity = '0';
      t.style.transform = 'translateY(12px) scale(0.95)';
      t.style.transition = 'all 0.3s ease';
      setTimeout(() => t.remove(), 300);
    }, 2400);
  }

  /* ============================================================
     TMDB API ENGINE (WITH SAFE PROMISES & NULL GUARDS)
     ============================================================ */
  const SUPABASE_TMDB_URL = "https://hwuguxuqytxdslbuawdp.supabase.co/functions/v1/tmdb";
  const TMDB_IMAGE_BASE = "https://image.tmdb.org/t/p/";

  async function getStreamUrl(item, season = null, episode = null) {
  const params = {
    action: "stream",
    type: item.type === "series" ? "tv" : "movie",
    id: item.tmdbId
  };
  if (item.type === "series") {
    params.season = season;
    params.episode = episode;
  }
  const query = new URLSearchParams(params);
  const response = await fetch(
    `${SUPABASE_TMDB_URL}?${query.toString()}`
  );
  if (!response.ok) {
    throw new Error(`Stream error ${response.status}`);
  }
  const data = await response.json();
  if (!data.embed_url) {
    throw new Error("Stream URL not found");
  }
  return data.embed_url;
}
  function tmdbImage(path, size = "w780") {
    return path ? `${TMDB_IMAGE_BASE}${size}${path}` : "";
  }
  async function tmdbRequest(action, params = {}) {
    const query = new URLSearchParams({ action, ...params });
    const response = await fetch(`${SUPABASE_TMDB_URL}?${query.toString()}`);
    if (!response.ok) throw new Error(`TMDB error ${response.status}`);
    return await response.json();
  }

  const TMDB_GENRES = {
    28: "Action", 12: "Adventure", 16: "Animation", 35: "Comedy",
    80: "Crime", 99: "Documentary", 18: "Drama", 14: "Fantasy",
    27: "Horror", 9648: "Mystery", 10749: "Romance", 878: "Sci-Fi", 53: "Thriller"
  };

  function convertTMDBTitle(item, forcedType = null) {
    const isSeries = forcedType === "series" || item.media_type === "tv" || !!item.name;
    const title = isSeries ? item.name : item.title;
    const date = isSeries ? item.first_air_date : item.release_date;
    const language = String(item.original_language || "").toLowerCase();

    let category = "hollywood";
    if (isSeries) {
      if (language === "ko") category = "kdrama";
      else if (language === "ja") category = "anime";
      else category = "webseries";
    } else {
      if (["hi", "ta", "te", "ml", "kn"].includes(language)) category = "bollywood";
    }

    const genres = Array.isArray(item.genre_ids)
      ? item.genre_ids.map(id => TMDB_GENRES[id]).filter(Boolean)
      : (Array.isArray(item.genres) ? item.genres.map(g => g.name || g).filter(Boolean) : ["Drama"]);

    return {
      id: `${isSeries ? "tv" : "movie"}-${item.id}`,
      tmdbId: item.id,
      title: title || "Untitled",
      type: isSeries ? "series" : "movie",
      category,
      tagline: item.overview ? item.overview.split(".")[0] : "A spectacular streaming journey.",
      rating: Number(item.vote_average || 8.0).toFixed(1),
      year: date ? Number(date.slice(0, 4)) : 2026,
      duration: item.runtime ? `${Math.floor(item.runtime / 60)}h ${item.runtime % 60}m` : (isSeries ? 'Series' : '2h 10m'),
      age: item.adult ? "A 18+" : "U/A 13+",
      languages: language ? [language.toUpperCase(), 'English'] : ["English"],
      genres: genres.length ? genres : ["Action", "Drama"],
      quality: ["4K", "HD"],
      cast: item.credits && item.credits.cast ? item.credits.cast.slice(0, 4).map(c => c.name) : ["Adrian Cole", "Nina Marsh"],
      director: item.credits && item.credits.crew ? (item.credits.crew.find(c => c.job === 'Director')?.name || 'Cinema Studio') : 'Marcus Feld',
      description: item.overview || "Stream this blockbuster exclusively on HPSTREAM in stunning 4K and Dolby Audio.",
      isTrending: item.vote_average > 7.4,
      isNew: true,
      trailerLen: "2:15",
      teaserLen: "0:45",
      poster: tmdbImage(item.poster_path, "w780") || primaryGenreColor({ genres }).img,
      backdrop: tmdbImage(item.backdrop_path, "w1280") || tmdbImage(item.poster_path, "w780"),
      youtubeTrailer: 'https://www.youtube.com/embed/dQw4w9WgXcQ?autoplay=1',
      seasons: []
    };
  }

  async function getTMDBMovie(id) {
    try {
      const data = await tmdbRequest("movie", { id: String(id) });
      return convertTMDBTitle(data, "movie");
    } catch {
      return byId[id] || INITIAL_TITLES[0];
    }
  }

  async function getTMDBTV(id) {
    try {
      const data = await tmdbRequest("tv", { id: String(id) });
      const converted = convertTMDBTitle(data, "series");
      if (Array.isArray(data.seasons)) {
        converted.seasons = data.seasons.filter(s => s.season_number > 0).map(s => ({
          season: s.season_number,
          name: s.name,
          episodes: [
            ep(1, `${s.name || 'Season ' + s.season_number} Premiere`, '48m', 'The story begins with unexpected twists across the city.')
          ]
        }));
      }
      return converted;
    } catch {
      return byId[id] || INITIAL_TITLES[2];
    }
  }

  async function searchTMDB(query) {
    if (!query || !query.trim()) return [];
    try {
      const data = await tmdbRequest("search", { query: query.trim(), page: '1' });
      return (data.results || [])
        .filter(item => !item.adult)
        .map(item => convertTMDBTitle(item, item.media_type === "tv" ? "series" : "movie"));
    } catch (e) {
      console.warn('TMDB search error:', e);
      return [];
    }
  }

  async function loadHPStreamTMDB() {
    try {
      const [trendingData, popularData] = await Promise.allSettled([
        tmdbRequest("trending", { page: '1' }),
        tmdbRequest("popular", { page: '1' })
      ]);

      const freshItems = [];
      if (trendingData.status === 'fulfilled' && trendingData.value && Array.isArray(trendingData.value.results)) {
        freshItems.push(...trendingData.value.results.map(i => convertTMDBTitle(i)));
      }
      if (popularData.status === 'fulfilled' && popularData.value && Array.isArray(popularData.value.results)) {
        freshItems.push(...popularData.value.results.map(i => convertTMDBTitle(i, 'movie')));
      }

      if (freshItems.length) {
        // Merge without duplicates
        const map = new Map();
        [...freshItems, ...INITIAL_TITLES].forEach(item => {
          if (item && !map.has(item.id)) {
            map.set(item.id, item);
          }
        });
        TITLES = Array.from(map.values());
        rebuildIndex();
        render();
      }
    } catch (error) {
      console.info('Using local high-speed master catalog.');
    }
  }

  /* ============================================================
     UNIFIED EVENT LISTENERS (NO DUPLICATES)
     ============================================================ */
  document.addEventListener('click', async (e) => {
    const el = e.target.closest('[data-action]');
    if (!el) return;

    const action = el.dataset.action;
    const id = el.dataset.id;

    switch (action) {
      case 'go-home':
        navigate('home');
        break;

      case 'go-home-browse':
        navigate('browse', {});
        break;

      case 'open-detail': {
        const item = byId[id];
        if (!item) return;

        if (item.type === 'series') {
          if (!item.seasons || !item.seasons.length) {
            try {
              const cleanId = String(id).replace(/^tv-/, '');
              const tvDetails = await getTMDBTV(cleanId);
              if (tvDetails && tvDetails.seasons && tvDetails.seasons.length) {
                item.seasons = tvDetails.seasons;
              }
            } catch (err) {
              console.warn('Series detail fallback active:', err);
            }
          }
          navigate('series', { id: item.id });
        } else {
          navigate('movie', { id: item.id });
        }
        break;
      }

      case 'player-toggle-play': {
        const plays = document.querySelectorAll('.player-bottom-left .play-icon');
        const pauses = document.querySelectorAll('.player-bottom-left .pause-icon');
        if (plays.length && pauses.length) {
          const isPaused = pauses[0].style.display === 'none';
          plays.forEach(p => p.style.display = isPaused ? 'none' : 'block');
          pauses.forEach(p => p.style.display = isPaused ? 'block' : 'none');
          showToast(isPaused ? '▶ Playback Resumed' : '⏸ Playback Paused');
        }
        break;
      }

      case 'player-seek-back': {
        state.playerPct = Math.max(0, (state.playerPct || 15) - 5);
        document.getElementById('playerTimelineProgress').style.width = state.playerPct + '%';
        document.getElementById('playerTimelineHandle').style.left = state.playerPct + '%';
        const curTimeStr = Math.floor((state.playerPct / 100) * 115) + ':20';
        document.getElementById('playerCurrentTime').textContent = curTimeStr;
        showToast('⏪ Rewound 10s');
        break;
      }

      case 'player-seek-fw': {
        state.playerPct = Math.min(100, (state.playerPct || 15) + 5);
        document.getElementById('playerTimelineProgress').style.width = state.playerPct + '%';
        document.getElementById('playerTimelineHandle').style.left = state.playerPct + '%';
        const curTimeStr = Math.floor((state.playerPct / 100) * 115) + ':20';
        document.getElementById('playerCurrentTime').textContent = curTimeStr;
        showToast('⏩ Fast Forwarded 10s');
        break;
      }

      case 'player-timeline-click': {
        const container = document.getElementById('playerTimeline');
        if (container && e.clientX) {
          const rect = container.getBoundingClientRect();
          const pct = Math.min(100, Math.max(0, Math.round(((e.clientX - rect.left) / rect.width) * 100)));
          state.playerPct = pct;
          const progBar = document.getElementById('playerTimelineProgress');
          const handle = document.getElementById('playerTimelineHandle');
          if (progBar) progBar.style.width = pct + '%';
          if (handle) handle.style.left = pct + '%';
          const curTimeStr = Math.floor((pct / 100) * 115) + ':20';
          const timeElem = document.getElementById('playerCurrentTime');
          if (timeElem) timeElem.textContent = curTimeStr;
        }
        break;
      }

      case 'player-skip-intro': {
        state.playerPct = Math.min(100, (state.playerPct || 15) + 12);
        document.getElementById('playerTimelineProgress').style.width = state.playerPct + '%';
        document.getElementById('playerTimelineHandle').style.left = state.playerPct + '%';
        const curTimeStr = Math.floor((state.playerPct / 100) * 115) + ':20';
        document.getElementById('playerCurrentTime').textContent = curTimeStr;
        showToast('⏭ Intro Skipped');
        document.getElementById('skipIntroBtn').style.display = 'none';
        break;
      }

      case 'player-toggle-mute': {
        const volSlider = document.getElementById('playerVolSlider');
        const volOn = document.querySelector('#playerMuteBtn .vol-on');
        const volOff = document.querySelector('#playerMuteBtn .vol-off');
        if (volSlider) {
          const isMute = volSlider.value > '0';
          volSlider.value = isMute ? '0' : '100';
          if (volOn) volOn.style.display = isMute ? 'none' : 'block';
          if (volOff) volOff.style.display = isMute ? 'block' : 'none';
          showToast(isMute ? '🔇 Volume Muted' : '🔊 Volume Restored');
        }
        break;
      }

      case 'player-toggle-speed-menu': {
        document.getElementById('playerSpeedDropdown')?.classList.toggle('open');
        break;
      }

      case 'player-set-speed': {
        const spd = el.dataset.speed;
        const lbl = document.getElementById('playerSpeedLabel');
        if (lbl) lbl.textContent = parseFloat(spd).toFixed(1) + 'x';
        document.querySelectorAll('.speed-opt').forEach(opt => {
          opt.classList.toggle('active', opt.dataset.speed === spd);
        });
        document.getElementById('playerSpeedDropdown')?.classList.remove('open');
        showToast(`Playback Speed set to ${spd}x`);
        break;
      }

      case 'player-toggle-theater': {
        const shell = document.getElementById('hpVideoPlayer');
        if (shell) {
          const tMode = shell.classList.toggle('theater-mode');
          showToast(tMode ? '🎭 Theater Mode Enabled' : '🎭 Theater Mode Disabled');
        }
        break;
      }

      case 'player-toggle-pip': {
        showToast('📺 Picture-In-Picture activated');
        break;
      }

      case 'player-toggle-fullscreen': {
        toggleFullscreen();
        break;
      }

      case 'go-watch': {
        navigate('watch', { id });
        state.playerPct = 12;
        break;
      }

      case 'go-watch-episode': {
        navigate('watch', {
          id,
          season: parseInt(el.dataset.season, 10) || 1,
          ep: parseInt(el.dataset.ep, 10) || 1
        });
        state.playerPct = 5;
        break;
      }

      case 'series-season':
        state.params = { id: el.dataset.id, season: parseInt(el.dataset.season, 10) };
        render();
        break;

      case 'nav-cat': {
        const cat = el.dataset.cat;
        document.getElementById('mobile-nav')?.classList.remove('open');
        if (cat === 'genres') {
          navigate('browse', { title: 'Explore Genres' });
          setTimeout(() => document.getElementById('filtersPanel')?.classList.add('open'), 60);
        } else if (cat === 'all') {
          navigate('browse', { category: 'all', title: 'All Titles in 4K' });
        } else {
          const match = CATS.find(c => c.key === cat);
          navigate('browse', { category: cat, title: match ? match.label : 'Explore' });
        }
        break;
      }

      case 'genre-click':
        navigate('browse', { genre: el.dataset.genre });
        break;

      case 'see-trending':
        navigate('browse', { title: 'Trending Worldwide' });
        state.activeFilters = { sort: 'trending' };
        render();
        break;

      case 'see-recent':
        navigate('browse', { title: 'Newly Added Cinema' });
        state.activeFilters = { sort: 'latest' };
        render();
        break;

      case 'see-recommended':
        navigate('browse', { title: 'Recommended Cinema' });
        break;

      case 'popular-tab':
        state.popularCat = el.dataset.cat;
        render();
        break;

      case 'toggle-watchlist': {
        if (state.watchlist.has(id)) {
          state.watchlist.delete(id);
          showToast('Removed from Watchlist');
        } else {
          state.watchlist.add(id);
          showToast('Added to Watchlist');
        }
        saveWatchlist();
        render();
        break;
      }

      case 'remove-watchlist':
        state.watchlist.delete(id);
        saveWatchlist();
        showToast('Removed from Watchlist');
        render();
        break;

      case 'go-watchlist':
        document.getElementById('mobile-nav')?.classList.remove('open');
        navigate('watchlist');
        break;

      case 'profile-click':
        document.getElementById('mobile-nav')?.classList.remove('open');
        state.isLoggedIn ? navigate('profile') : openAuthModal('login');
        break;

      case 'logout':
        state.isLoggedIn = false;
        localStorage.removeItem(STORAGE_KEYS.USER);
        navigate('home');
        showToast('Signed out successfully');
        break;

      case 'scroll-continue':
        document.getElementById('pv-continue')?.scrollIntoView({ behavior: 'smooth', block: 'start' });
        break;

      case 'open-search':
        navigate('browse', { title: 'Search 4K Cinema' });
        setTimeout(() => document.getElementById('searchInput')?.focus(), 100);
        break;

      case 'clear-search':
        state.params = {};
        state.activeFilters = {};
        navigate('browse', {});
        break;

      case 'rail-scroll': {
        const rail = document.getElementById(el.dataset.target);
        if (rail) rail.scrollBy({ left: 340 * parseInt(el.dataset.dir, 10), behavior: 'smooth' });
        break;
      }

      case 'hero-prev':
        state.heroIndex = (state.heroIndex - 1 + getHeroPool().length) % getHeroPool().length;
        render();
        resetHeroTimer();
        break;

      case 'hero-next':
        state.heroIndex = (state.heroIndex + 1) % getHeroPool().length;
        render();
        resetHeroTimer();
        break;

      case 'hero-goto':
        state.heroIndex = parseInt(el.dataset.idx, 10);
        render();
        resetHeroTimer();
        break;

      case 'open-trailer':
        openVideoModal(byId[id], el.dataset.kind);
        break;

      case 'open-mobile-nav':
        document.getElementById('mobile-nav')?.classList.add('open');
        break;

      case 'close-mobile-nav':
        document.getElementById('mobile-nav')?.classList.remove('open');
        break;

      case 'toggle-filters':
        document.getElementById('filtersPanel')?.classList.toggle('open');
        break;

      case 'share': {
        const item = byId[id] || INITIAL_TITLES[0];
        const shareUrl = window.location.href;
        if (navigator.share) {
          navigator.share({ title: item.title, text: `Watch ${item.title} on HPSTREAM in 4K:`, url: shareUrl }).catch(() => {});
        } else if (navigator.clipboard) {
          navigator.clipboard.writeText(shareUrl).then(() => showToast('Stream link copied to clipboard!'));
        }
        break;
      }

      case 'auth-switch':
        openAuthModal(el.dataset.mode);
        break;

      case 'demo-login':
        state.isLoggedIn = true;
        localStorage.setItem(STORAGE_KEYS.USER, 'demo_user');
        closeModal();
        navigate('profile');
        showToast('Signed in successfully!');
        break;

      case 'close-modal':
        closeModal();
        break;

      case 'switch-server': {
        const serverKey = el.dataset.server;
        if (!STREAM_SERVERS[serverKey]) break;
        playerState.currentServer = serverKey;
        // Update active tab UI
        document.querySelectorAll('.server-tab').forEach(btn => {
          btn.classList.toggle('active', btn.dataset.server === serverKey);
        });
        loadPlayerServer(serverKey);
        showToast(`${STREAM_SERVERS[serverKey].name} server selected`);
        break;
      }

      case 'switch-lang': {
        const langName = el.dataset.langName;
        // Update active tab UI
        document.querySelectorAll('.lang-tab').forEach(btn => {
          btn.classList.toggle('active', btn.dataset.langName === langName);
        });
        playerState.currentLang = langName;
        showToast(`${langName} audio / subtitle selected`);
        break;
      }

      default:
        break;
    }
  });

  /* ============================================================
     LIVE SEARCH & FILTER INPUT LISTENER
     ============================================================ */
  let searchDebounceTimer = null;
  document.addEventListener('input', (e) => {
    if (e.target.id === 'searchInput') {
      const q = e.target.value.trim();
      state.params = { ...state.params, q };
      const box = document.getElementById('suggestBox');

      clearTimeout(searchDebounceTimer);
      searchDebounceTimer = setTimeout(async () => {
        if (!q) {
          if (box) box.innerHTML = '';
          render();
          document.getElementById('searchInput')?.focus();
          return;
        }

        // Live suggestions
        const localMatches = TITLES.filter(t => t.title.toLowerCase().includes(q.toLowerCase())).slice(0, 5);
        if (box) {
          box.innerHTML = localMatches.length ? `
            <div class="suggest-drop">
              ${localMatches.map(item => `
                <div class="suggest-item" data-action="open-detail" data-id="${item.id}">
                  <div class="sp" style="background-image:url('${item.poster || primaryGenreColor(item).img}');"></div>
                  <div>
                    <div class="st">${esc(item.title)}</div>
                    <div class="ss">${item.year || '2026'} · ${item.type === 'series' ? 'Series' : 'Movie'}</div>
                  </div>
                </div>`).join('')}
            </div>` : '';
        }

        // Search TMDB in background if local results are few
        if (localMatches.length < 3) {
          try {
            const tmdbResults = await searchTMDB(q);
            if (tmdbResults.length) {
              const map = new Map();
              [...tmdbResults, ...TITLES].forEach(item => map.set(item.id, item));
              TITLES = Array.from(map.values());
              rebuildIndex();
            }
          } catch {}
        }
        render();
        const input = document.getElementById('searchInput');
        if (input) {
          input.focus();
          input.setSelectionRange(input.value.length, input.value.length);
        }
      }, 300);
    }

    if (e.target.dataset && e.target.dataset.fkey) {
      state.activeFilters = state.activeFilters || {};
      state.activeFilters[e.target.dataset.fkey] = e.target.value;
      const curScroll = window.scrollY;
      render();
      document.getElementById('filtersPanel')?.classList.add('open');
      window.scrollTo(0, curScroll);
    }
  });

  /* ============================================================
     FULLSCREEN CONTROLS ENGINE
     ============================================================ */
  function toggleFullscreen() {
    const shell = document.getElementById('hpVideoPlayer');
    if (!shell) return;

    const isFs = !!(
      document.fullscreenElement ||
      document.webkitFullscreenElement ||
      document.mozFullScreenElement ||
      document.msFullscreenElement
    );

    if (!isFs) {
      const req = shell.requestFullscreen || shell.webkitRequestFullscreen || shell.mozRequestFullScreen || shell.msRequestFullscreen;
      if (req) {
        req.call(shell).catch(err => {
          console.warn('Fullscreen request failed:', err);
          showToast('⚠️ Fullscreen not supported or blocked');
        });
      }
    } else {
      const exit = document.exitFullscreen || document.webkitExitFullscreen || document.mozCancelFullScreen || document.msExitFullscreen;
      if (exit) {
        exit.call(document).catch(err => {
          console.warn('Exit fullscreen failed:', err);
        });
      }
    }
  }

  // Synchronize fullscreen state across entire document
  ['fullscreenchange', 'webkitfullscreenchange', 'mozfullscreenchange', 'MSFullscreenChange'].forEach(evt => {
    document.addEventListener(evt, () => {
      const isFs = !!(
        document.fullscreenElement ||
        document.webkitFullscreenElement ||
        document.mozFullScreenElement ||
        document.msFullscreenElement
      );
      const shell = document.getElementById('hpVideoPlayer');
      if (shell) {
        shell.classList.toggle('is-fullscreen', isFs);
      }
      document.querySelectorAll('.fs-open').forEach(i => i.style.display = isFs ? 'none' : '');
      document.querySelectorAll('.fs-close').forEach(i => i.style.display = isFs ? '' : 'none');
      showToast(isFs ? '⛶ Fullscreen Entered' : '⛶ Fullscreen Exited');
    });
  });

  /* Keyboard Accessibility & Video Player Shortcuts */
  document.addEventListener('keydown', (e) => {
    const isInput = ['INPUT', 'TEXTAREA', 'SELECT'].includes(document.activeElement?.tagName);
    
    if (e.key === 'Escape') {
      closeModal();
      document.getElementById('mobile-nav')?.classList.remove('open');
      if (document.fullscreenElement) document.exitFullscreen();
    }
    if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'k') {
      e.preventDefault();
      navigate('browse', { title: 'Search 4K Cinema' });
      setTimeout(() => document.getElementById('searchInput')?.focus(), 80);
    }

    // Player specific shortcuts (if not typing in input)
    if (!isInput && (state.view === 'watch' || state.page === 'watch')) {
      const key = e.key.toLowerCase();
      if (e.key === ' ' || key === 'k') {
        e.preventDefault();
        const btn = document.querySelector('#playerPlayHero');
        if (btn) btn.click();
      } else if (key === 'f') {
        e.preventDefault();
        toggleFullscreen();
      } else if (key === 'm') {
        e.preventDefault();
        const btn = document.getElementById('playerMuteBtn');
        if (btn) btn.click();
      } else if (key === 't') {
        e.preventDefault();
        const shell = document.getElementById('hpVideoPlayer');
        if (shell) {
          const tMode = shell.classList.toggle('theater-mode');
          showToast(tMode ? '🎭 Theater Mode Enabled' : '🎭 Theater Mode Disabled');
        }
      } else if (key === 'j' || e.key === 'ArrowLeft') {
        e.preventDefault();
        const btn = document.querySelector('[data-action="player-seek-back"]');
        if (btn) btn.click();
      } else if (key === 'l' || e.key === 'ArrowRight') {
        e.preventDefault();
        const btn = document.querySelector('[data-action="player-seek-fw"]');
        if (btn) btn.click();
      }
    }
  });

  /* Scroll Header State */
  window.addEventListener('scroll', () => {
    const header = document.getElementById('site-header');
    if (header) {
      header.classList.toggle('scrolled', window.scrollY > 15);
    }
  }, { passive: true });

  /* ============================================================
     INITIALIZATION
     ============================================================ */
  function initHPStream() {
    const canvas = document.getElementById('ambient-canvas');
    if (canvas && canvas.getContext) {
      const ctx = canvas.getContext('2d');
      // Set the canvas dimensions
      canvas.width = window.innerWidth;
      canvas.height = window.innerHeight;

      // Set the animation frame for the lighting effect
      function draw() {
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        ctx.fillStyle = 'rgba(229, 9, 20, 0.08)';
        ctx.beginPath();
        ctx.arc(canvas.width / 2, canvas.height / 2, 400, 0, Math.PI * 2);
        ctx.fill();
        requestAnimationFrame(draw);
      }
      draw();

      window.addEventListener('resize', () => {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
      }, { passive: true });
    }


    render();
    resetHeroTimer();
    loadHPStreamTMDB();
    console.log('%c✓ HPSTREAM 3D Engine Initialized (60fps Ultra)', 'color:#e50914;font-weight:bold;font-size:14px;');
  }

  // Launch when DOM is ready
  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initHPStream);
  } else {
    initHPStream();
  }

})();
